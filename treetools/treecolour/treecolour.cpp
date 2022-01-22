// Copyright (c) 2022
// Commonwealth Scientific and Industrial Research Organisation (CSIRO)
// ABN 41 687 119 230
//
// Author: Thomas Lowe
#include "treelib/treeutils.h"
#define STB_IMAGE_IMPLEMENTATION
#include "treelib/imageread.h"
#include <raylib/rayforestgen.h>
#include <raylib/raytreegen.h>
#include <raylib/rayparse.h>
#include <raylib/raycloud.h>
#include <raylib/rayrenderer.h>
#include "raylib/raytreegen.h"
#include <cstdlib>
#include <iostream>

void usage(int exit_code = 1)
{
  std::cout << "Colour a tree file from an image" << std::endl;
  std::cout << "usage:" << std::endl;
  std::cout << "treecolour forest.txt LAI_image.hdr 10.3,-12.4,0.2 - applies image colour to the tree file" << std::endl;
  std::cout << "                                                     at min coordinate 10.3,-12.4 and pixel width 0.2m" << std::endl;
  exit(exit_code);
}

// Read in a ray cloud and convert it into an array for topological optimisation
int main(int argc, char *argv[])
{
  ray::FileArgument forest_file, image_file;
  ray::Vector3dArgument coord;
  bool parsed = ray::parseCommandLine(argc, argv, {&forest_file, &image_file, &coord});
  if (!parsed)
  {
    usage();
  }

  ray::ForestStructure forest;
  if (!forest.load(forest_file.name()))
  {
    usage();
  }  

  std::cout << "reading image: " << image_file.name() << std::endl;
  std::string ext = image_file.nameExt();

  const char *image_name = image_file.name().c_str();
  stbi_set_flip_vertically_on_load(1);
  int width, height, num_channels;
  unsigned char *image_data = 0;
  float *image_dataf = 0;
  bool is_hdr = image_file.nameExt() == "hdr";
  if (is_hdr)
    image_dataf = stbi_loadf(image_name, &width, &height, &num_channels, 0);
  else 
    image_data = stbi_load(image_name, &width, &height, &num_channels, 0);

  int tree_radius_id = -1;
  for (size_t i = 0; i<forest.trees[0].attributes().size(); i++)
    if (forest.trees[0].attributes()[i] == "tree_radius")
      tree_radius_id = i;
  double trunk_to_tree_radius_scale = 10.0;
  bool trunks_only = forest.trees[0].segments().size() == 1;
  if (tree_radius_id == -1 && trunks_only)
    std::cout << "Warning: tree file does not contain tree radii, so they are estimated as " << trunk_to_tree_radius_scale << " times the trunk radius." << std::endl;

  for (auto &tree: forest.trees)
  {
    tree.attributes().push_back("red");
    tree.attributes().push_back("green");
    tree.attributes().push_back("blue");
    Eigen::Vector2d centre(tree.root()[0], tree.root()[1]);
    double rad = tree_radius_id == -1 ? tree.segments()[0].radius * trunk_to_tree_radius_scale : tree.segments()[0].attributes[tree_radius_id];
    // get the radius:
    if (tree_radius_id == -1 && trunks_only==false) // no tree radius specified, so estimate it from the branches
    {
      const double big = 1e10;
      Eigen::Vector3d minbound(big,big,big), maxbound(-big,-big,-big);
      for (auto &segment: tree.segments())
      {
        minbound = ray::minVector(minbound, segment.tip);
        maxbound = ray::maxVector(maxbound, segment.tip);
      }
      Eigen::Vector3d extent = (maxbound - minbound)/2.0;
      rad = 0.5 * (extent[0] + extent[1]); // the mean diameter of the points
    }


    Eigen::Vector2d minpos = centre - Eigen::Vector2d(rad, rad);
    Eigen::Vector2d maxpos = centre + Eigen::Vector2d(rad, rad);
    Eigen::Vector2d coords(coord.value()[0], coord.value()[1]);
    Eigen::Vector2i boxmin = ((minpos - coords) / coord.value()[2]).cast<int>();
    Eigen::Vector2i boxmax = ((maxpos - coords) / coord.value()[2]).cast<int>();
    Eigen::Vector3d colour(0,0,0);
    int count = 0;
    for (int i = std::max(0, boxmin[0]); i<=std::min(boxmax[0], width-1); i++)
    {
      for (int j = std::max(0, boxmin[1]); j<=std::min(boxmax[1], height-1); j++)
      {
        Eigen::Vector2d pos = Eigen::Vector2d(i + 0.5,j + 0.5)*coord.value()[2] + coords;
        if ((pos - centre).norm() > rad)
          continue;
        const int index = num_channels*(i + width*j);
        if (is_hdr)
        {
          if (image_dataf[index] > 0.f || image_dataf[index+1] > 0.f || image_dataf[index+2] > 0.f)
          {
            colour[0] += (double)image_dataf[index];
            colour[1] += (double)image_dataf[index+1];
            colour[2] += (double)image_dataf[index+2];      
            count++;  
          }
        }
        else
        {
          if (image_data[index] > 0 || image_data[index+1] > 0 || image_data[index+2] > 0)
          {
            colour[0] += (double)image_data[index];
            colour[1] += (double)image_data[index+1];
            colour[2] += (double)image_data[index+2];   
            count++;  
          }   
        }
      }
    }
    if (count > 0)
      colour /= (double)count;
    for (auto &segment: tree.segments())
    {
      segment.attributes.push_back(colour[0]);
      segment.attributes.push_back(colour[1]);
      segment.attributes.push_back(colour[2]);
    }
  }
  forest.save(forest_file.nameStub() + "_coloured.txt");
}

