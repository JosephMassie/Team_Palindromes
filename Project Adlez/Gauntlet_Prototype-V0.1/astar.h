#pragma once
#include <stdio.h>
#include "graph.h"
#include "templatehashmap.h"
#include "templatevector.h"

// comment this line to make the A* calculate instantly.
//#define COROUTINE_SEGMENTATION

struct Astar
{
	float dist_between(GraphNode * a, GraphNode * b)
	{
	//	return a->getArea().getCenter().distance(b->getArea().getCenter());
		return 15;
	}

	float heuristic_cost_estimate(GraphNode * start, GraphNode * goal)
	{
		// arbitrarily large number
		return dist_between(start, goal)*10;
	}

	GraphNode * getLowestFrom(TemplateVector<GraphNode*> & a_openSet, TemplateHashMap<GraphNode*, float> & a_scoring)
	{
		int index = 0;
		float lowest = *a_scoring.getByKey(a_openSet.get(0));
		float score;
		for(int i = 1; i < a_openSet.size(); ++i)
		{
			score = *a_scoring.getByKey(a_openSet.get(i));
			if(score < lowest)
			{
				index = i;
				lowest = score;
			}
		}
		return a_openSet.get(index);
	}

	//function reconstruct_path(came_from, current_node)
	void reconstruct_path(TemplateHashMap<GraphNode*, GraphNode*> * came_from, GraphNode * current_node, TemplateVector<GraphNode*> * list)
	{
		////if came_from[current_node] is set
		//if(came_from->getByKey(current_node) != 0)
		//{
		//	GraphNode * nodeBeforeThisOne = *came_from->getByKey(current_node);
		//	//p := reconstruct_path(came_from, came_from[current_node])
		//	reconstruct_path(came_from, nodeBeforeThisOne, list);
		//	//return (p + current_node)
		//	list->add(nodeBeforeThisOne);
		//}
		////else
		//	//return current_node
		GraphNode ** previous = &current_node;
		do{
			if(list->indexOf(*previous) >= 0)
				break;
			list->add(*previous);
			previous = came_from->getByKey(*previous);
		}while(previous);
	}

	TemplateVector<GraphNode*> closedset;
	TemplateVector<GraphNode*> openset;
	TemplateHashMap<GraphNode*, GraphNode*> came_from;
	TemplateHashMap<GraphNode*, float> g_score;
	TemplateHashMap<GraphNode*, float> h_score;
	TemplateHashMap<GraphNode*, float> f_score;

	/*void glDrawNodeData(GraphNode * n)
	{
		char buffer[256];
		V2DF p;
		V2DF ln(0,-.3);
		float *g, *h, *f;
		p = n->getArea().getCenter();
		g = g_score.getByKey(n);
		h = h_score.getByKey(n);
		f = f_score.getByKey(n);
		if(g){
			sprintf_s(buffer,sizeof(buffer),"g %.2f",*g);
			p.difference(ln).glDrawString(buffer);
		}if(h){
			sprintf_s(buffer,sizeof(buffer),"h %.2f",*h);
			p.glDrawString(buffer);
		}if(f){
			sprintf_s(buffer,sizeof(buffer),"f %.2f",*f);
			p.sum(ln).glDrawString(buffer);
		}
		GraphNode ** from = came_from.getByKey(n);
		if(from){
			p.glDrawTo((*from)->getArea().getCenter());
		}
	}
*/
	/*void glDraw()
	{
		int red = 0x0000ff, green = 0x00ff00, white = 0xffffff;
		GraphNode* n;
		glColor3ubv((GLubyte*)&white);
		for(int i = 0; i < closedset.size(); ++i)
		{
			n = closedset.get(i);
			glDrawNodeData(n);		
		}
		glColor3ubv((GLubyte*)&green);
		for(int i = 0; i < openset.size(); ++i)
		{
			n = openset.get(i);
			glDrawNodeData(n);		
		}
	}*/

	static const int INIT = 0, RUNNING = 1, DONE = 2;
	int state;

	// algorithm found at http://en.wikipedia.org/wiki/A*_search_algorithm (March 5, 2012)
	Astar():state(INIT){}
	//function A*(start,goal)
	int findPath(GraphNode * start, GraphNode * goal, TemplateVector<GraphNode*> * a_output)
	{
#ifdef COROUTINE_SEGMENTATION
	switch(state){case INIT:
#endif
		a_output->clear();
		//closedset := the empty set    // The set of nodes already evaluated.
		closedset.clear();
		//openset := {start}    // The set of tentative nodes to be evaluated, initially containing the start node
		openset.clear();
		openset.add(start);
		//came_from := the empty map    // The map of navigated nodes.
		came_from.clear();

		//g_score[start] := 0    // Cost from start along best known path.
		g_score.clear();
		g_score.set(start, 0);
		//h_score[start] := heuristic_cost_estimate(start, goal)
		h_score.clear();
		h_score.set(start, heuristic_cost_estimate(start, goal));
		//f_score[start] := g_score[start] + h_score[start]    // Estimated total cost from start to goal through y.
		f_score.clear();
		f_score.set(start, *g_score.getByKey(start) + *h_score.getByKey(start));
#ifdef COROUTINE_SEGMENTATION
	state = RUNNING;break;case RUNNING:
#endif
		//while openset is not empty
#ifdef COROUTINE_SEGMENTATION
		if(openset.size() > 0)
#else
		while(openset.size() > 0)
#endif
		{
			//current := the node in openset having the lowest f_score[] value
			GraphNode * current = getLowestFrom(openset, f_score);
			//if current = goal
			if(current == goal)
			{
				//return reconstruct_path(came_from, came_from[goal])
				reconstruct_path(&came_from, goal, a_output);
				state = DONE;
				return state;
			}
			//remove current from openset
			openset.removeData(current);
			//add current to closedset
			closedset.add(current);
			//foreach neighbor in neighbor_nodes(current)

			
			for(int i = 0; i < current->getNeighborCount(); ++i)
			{
				GraphNode * neighbor = current->getNeighbors()->get(i).neighbor;
				//if neighbor in closedset
				if(closedset.indexOf(neighbor) >= 0)
				{
					//continue
					continue;
				}
				//tentative_g_score := g_score[current] + dist_between(current,neighbor)
				float tentative_g_score = *g_score.getByKey(current) + dist_between(current, neighbor);

				bool tentative_is_better;
				//if neighbor not in openset
				if(openset.indexOf(neighbor) < 0)
				{
					//add neighbor to openset
					openset.add(neighbor);
					//h_score[neighbor] := heuristic_cost_estimate(neighbor, goal)
					h_score.set(neighbor, heuristic_cost_estimate(neighbor, goal));
					//tentative_is_better := true
					tentative_is_better = true;
				}
				//else if tentative_g_score < g_score[neighbor]
				else if(tentative_g_score < *g_score.getByKey(neighbor))
				{
					//tentative_is_better := true
					tentative_is_better = true;
				}
				//else
				else
				{
					//tentative_is_better := false
					tentative_is_better = false;
				}
				//if tentative_is_better = true
				if(tentative_is_better)
				{
					//came_from[neighbor] := current
					came_from.set(neighbor, current);
					//g_score[neighbor] := tentative_g_score
					g_score.set(neighbor, tentative_g_score);
					//f_score[neighbor] := g_score[neighbor] + h_score[neighbor]
					f_score.set(neighbor, *g_score.getByKey(neighbor) + *h_score.getByKey(neighbor));
				}
			}
		}
#ifdef COROUTINE_SEGMENTATION
		else state = DONE;break;}
#endif
		//return failure
		return state;
	}
};