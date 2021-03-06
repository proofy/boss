/* boss_nn.h    $Revision: 1.2 $ $Date: 2005/12/10 20:33:54 $
   Copyright (C) 2002 University of Bonn.
   http://www.ikp.uni-bonn.de/boss

   This program is free software; you can redistribute it and/or
   modify it under the terms of the GNU General Public License
   as published by the Free Software Foundation; either version 2
   of the License, or (at your option) any later version.

   This program is distributed in the hope that it will be useful,
   but WITHOUT ANY WARRANTY; without even the implied warranty of
   MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
   GNU General Public License for more details.

   You should have received a copy of the GNU General Public License
   along with this program; if not, write to the Free Software
   Foundation, Inc., 59 Temple Place - Suite 330,
   Boston, MA  02111-1307, USA.
*/

/**
 * @file boss_nn.h
   @author Philip Gross
*/

#ifndef BOSS_INTONATION_NN_H
#define BOSS_INTONATION_NN_H

//#include "dom/DOM.hpp"
#include "boss_utility/dom_tools.h"
#include "boss_intonation.h"
//#include "boss_nn.h"
#include <iostream>
#include <string>
#include <fstream>
#include <vector>
//#include <stdio.h>
//#include <stdlib.h>

/**
 * @addtogroup boss_intonation
 * @{
 */

/**
 * @brief Neural network class.
 */
class BOSS_NN {
public:
  BOSS_NN();
  ~BOSS_NN();
  bool LoadNN(const string & filename);
  bool SaveNN(const string & filename);
  vector<float> Calculate(const vector<float> & Input);
  float BOSS_NN::net(int Layer, int Neuron, const vector<float> & Input);
  void BackProp(vector<float> & DesiredOutput, vector<float> & Output);
  void Update();
  void Reset();
  struct t_neuron
  {
    vector<float> weight;
    float deltaw;
    float out;
  };
  struct t_layer
  {
    int NumberOfNeurons;
    int NumberOfWeights;
    vector<t_neuron> neuron;
  };
public: // Public attributes
  vector<t_layer> layer;
  int NumberOfLayers;
  float Learnrate;
  float Error;
  int m_InputNeurons;
};

/** @} */ // end of boss_intonation group
#endif

