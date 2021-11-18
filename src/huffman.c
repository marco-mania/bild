/* BILD - Wavelet based image compression
 * All rights reserved (since 2004). Marco Nelles.
 * <https://gitlab.com/wavelets/bild>
 *
 * This program is free software: you can redistribute it and/or modify
 * it under the terms of the GNU General Public License as published by
 * the Free Software Foundation, either version 3 of the License, or
 * (at your option) any later version.
 *
 * This program is distributed in the hope that it will be useful,
 * but WITHOUT ANY WARRANTY; without even the implied warranty of
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
 * GNU General Public License for more details.
 *
 * You should have received a copy of the GNU General Public License
 * along with this program.  If not, see <http://www.gnu.org/licenses/>.
 */

#include "huffman.h"

int huffman_frequency_compare(const void *elem1, const void *elem2)
{

  HuffmanNode *node1 = (HuffmanNode*)elem1;
  HuffmanNode *node2 = (HuffmanNode*)elem2;

  if (node1->frequency == node2->frequency) return 0;

  return node1->frequency < node2->frequency ? 1 : -1;

}

int huffman_symbol_compare(const void *elem1, const void *elem2 )
{

  return ((HuffmanNode*)elem1)->symbol > ((HuffmanNode*)elem2)->symbol ? 1 : -1;

}

HuffmanNode* huffman_pop_node(HuffmanNode *nodes[], const int i, const bool bRight)
{

  HuffmanNode *node = nodes[i];
  node->code = bRight;
  node->code_length = 1;
  return node;

}

void huffman_set_node_code(HuffmanNode *node)
{

  HuffmanNode *parent = node->parent;

  while (parent && parent->code_length)
  {

    node->code <<= 1;
    node->code |= parent->code;
    node->code_length++;
    parent = parent->parent;

  }

}

int huffman_get_tree(HuffmanNode nodes[], bool bSetCodes)
{

  HuffmanNode *pnodes[BYTE_MAX+1], *node;

  int node_count = 0;
  int i;

  for (i = 0; (i < BYTE_MAX+1) && (nodes[i].frequency); ++i)
    pnodes[node_count++] = &nodes[i];

  int nParentNode = node_count;
  int back_node = node_count-1;

  while (back_node > 0)
  {

    node = &nodes[nParentNode++];

    node->left_child = huffman_pop_node(pnodes, back_node--, 0);

    node->right_child = huffman_pop_node(pnodes, back_node--, 1);

    node->left_child->parent = node;
    node->right_child->parent = node;

    node->frequency = node->left_child->frequency + node->right_child->frequency;

    for (i = back_node; i >= 0; --i)
      if (pnodes[i]->frequency >= node->frequency) break;

    memmove(pnodes+i+2, pnodes+i+1, (back_node-i)*sizeof(HuffmanNode*));
    pnodes[i+1] = node;
    ++back_node;

  }

  if (bSetCodes)
    for (i = 0; i < node_count; ++i)
      huffman_set_node_code(&nodes[i]);

  return node_count;

}

void huffman_init_nodes(HuffmanNode nodes[])
{

  int i;
  for (i = 0; i < MAX_NODE_COUNT; ++i)
  {
    nodes[i].frequency = 0;
    nodes[i].symbol = 0;
    nodes[i].code = 0;
    nodes[i].code_length = 0;
    nodes[i].parent = NULL;
    nodes[i].left_child = NULL;
    nodes[i].right_child = NULL;
  }

}

void huffmanEncode(void *data, const int size, void *coded_data, int *coded_size, void *parameters)
{

  byte *d = data;

  byte *cd = coded_data;
  int coded_data_index = 0;

  HuffmanNode nodes[MAX_NODE_COUNT];
  huffman_init_nodes(nodes);

  int i;
  for (i = 0; i < BYTE_MAX+1; ++i) nodes[i].symbol = i;

  for (i = 0; i < size; ++i) ++nodes[d[i]].frequency;
  qsort(nodes, BYTE_MAX+1, sizeof(HuffmanNode), huffman_frequency_compare);

  uint32_t tmp = (uint32_t)size;
  memcpy(cd, &tmp, sizeof(uint32_t));
  coded_data_index += sizeof(uint32_t);

  int node_count = huffman_get_tree(nodes, 1);

  cd[coded_data_index++] = (byte)node_count-1;

  for (i = 0; i < node_count; ++i)
  {
    memcpy(&cd[coded_data_index], &nodes[i].frequency, sizeof(uint32_t));
    coded_data_index += sizeof(uint32_t);
    cd[coded_data_index++] = nodes[i].symbol;
  }

  qsort(nodes, BYTE_MAX+1, sizeof(HuffmanNode), huffman_symbol_compare);

  byte *p = &cd[coded_data_index];
  int coded_data_bit_index = 0;
  for (i = 0; i < size; ++i)
  {
    *(uint32_t*)(&p[coded_data_bit_index>>3]) |= nodes[d[i]].code << (coded_data_bit_index&7);
    coded_data_bit_index += nodes[d[i]].code_length;
  }

  *coded_size = coded_data_index+((coded_data_bit_index+7) >> 3);

}

void huffmanDecode(void *coded_data, const int coded_size, void *data, int *size)
{

  byte *d = data;
  int data_index = 0;

  byte *cd = coded_data;
  int coded_data_index = 0;

  uint32_t tmp;
  memcpy(&tmp, cd, sizeof(uint32_t));
  *size = (int)tmp;
  coded_data_index += sizeof(uint32_t);

  int node_count = (int)cd[coded_data_index++]+1;

  HuffmanNode nodes[MAX_NODE_COUNT];
  huffman_init_nodes(nodes);

  int i;
  for (i = 0; i < node_count; ++i)
  {
    memcpy(&nodes[i].frequency, &cd[coded_data_index], sizeof(uint32_t));
    coded_data_index += sizeof(uint32_t);
    nodes[i].symbol = cd[coded_data_index++];
  }

  huffman_get_tree(nodes, 0);

  HuffmanNode *pRoot = &nodes[0];
  while (pRoot->parent) pRoot = pRoot->parent;

  uint32_t nCode;
  byte *p = (byte*)&cd[coded_data_index];
  int coded_data_bit_index = 0;
  HuffmanNode *pNode;
  while (data_index < *size)
  {

    nCode = (*(uint32_t*)(&p[coded_data_bit_index>>3]))>>(coded_data_bit_index&7);
    pNode = pRoot;
    while (pNode->left_child)
    {
      pNode = (nCode&1) ? pNode->right_child : pNode->left_child;
      nCode >>= 1;
      ++coded_data_bit_index;
    }
    d[data_index++] = pNode->symbol;

  }

}
