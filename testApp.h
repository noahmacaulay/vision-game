#pragma once

#include "ofMain.h"
#include <vector>


#define UP_ARROW 357
#define DOWN_ARROW 359
#define LEFT_ARROW 356
#define RIGHT_ARROW 358
#define SPACE_BAR 32

#define LEFT_MOUSE 0
#define MIDDLE_MOUSE 1
#define RIGHT_MOUSE 2

#define NO_KEY_PRESSED -1

#define MY_PI 3.14159265

#define SOLID 0
#define MIRROR 1

#define CLOCKWISE 0
#define COUNTERCLOCKWISE 1


struct myPoint{

	float x;
	float y;

};

struct myColor{
	float r;
	float g;
	float b;
};

struct point_info{

	int poly_iter;
	int point_iter;
	float angle;
	float tan;

};

class myLine{
public:
	vector<float> dividers;
	vector<myColor> colors;

	myColor Color_at_Point(float point){
		for(int i = 0; i < dividers.size() - 1; i++){

			if (point >= dividers[i] && point < dividers[i+1]){
				return colors[i];
			}

		}
		return colors[dividers.size() - 1];

	}
	int Divider_at_Point(float point){
		for(int i = 0; i < dividers.size() - 1; i++){

			if (point >= dividers[i] && point < dividers[i+1]){
				return i;
			}

		}
		return dividers.size() - 1;



	}
};

struct myPolygon{
public:

	vector<myPoint> pointlist;

	vector<myLine> line_list; //The colours on each line of the polygon
	
	myColor default_line_color;

	myColor color;

	bool grabbed;

	int type;

	int orientation;

};

struct Light{

	myPoint position;
	myColor color;
	float radius; //Lights are circles(?)

};



//Simulates "multi-value" function returns
struct intersection_information{

	//Default values
intersection_information():
	polygon_num(-1),
	edge_num(-1),
	edge_distance(0),
	passed_target(false),
	hit_target(false),
	distance(2000),
	projection_position(0),
	passed_polygon_num(-1){}
	

	int polygon_num;
	int passed_polygon_num;
	int passed_point_num;
	int edge_num;
	float edge_distance;
	bool passed_target;
	bool hit_target;
	float distance;
	float projection_position;
};

class testApp : public ofBaseApp{
	public:
		void setup();
		void update();
		void draw();
		
		void keyPressed(int key);
		void keyReleased(int key);
		void mouseMoved(int x, int y);
		void mouseDragged(int x, int y, int button);
		void mousePressed(int x, int y, int button);
		void mouseReleased(int x, int y, int button);
		void windowResized(int w, int h);
		void dragEvent(ofDragInfo dragInfo);
		void gotMessage(ofMessage msg);

		void draw_Polygon(myPolygon polygon);
		myPoint playerpoint;
		myPoint player_direction;
		float player_movement_rate;
		float player_turning_rate;
		
		float view_angle;
		float view_angle_cos;
		float view_angle_sine;
		float view_angle_tan;
		int num_rays;
		
		float mouse_x;
		float mouse_y;


		float eye_radius;
		float pupil_depth;
		float sclera_depth;

		myLine player_viewpoint;
		void Add_View_Interval(int polygon_iter, int side_iter, float block_start, float block_end, float projection_start, float projection_end);

		vector<Light> light_list;
		int current_light_index;
		bool light_placement;

		vector<myPolygon> polylist;
		int current_poly_index;

		void Update_Polygon_Orientations();

		void Color_Line_Segment(int polygon_iter, int side_iter, float start, float end, myColor color);

		bool pressed[1024];
		int current_key_pressed;
		float last_time;

		myPoint screen_bottom_corner;

		float screen_height;
		float screen_width;

		vector<myColor> view_screen;
		vector<myColor> screen_shot;

		intersection_information Closest_Intersection(myPoint origin, myPoint direction, float distance_travelled, int num_reflections, bool mirrors_on, int destination_poly_index, int destination_point_index);
		bool polygonsIntersect(myPolygon polygon1, myPolygon polygon2);

		bool point_selected;
		int polygon_selected_index;
		int point_selected_index;

		myPoint original_selected_position;

		bool view_rays;

		bool t;

		int last_mirror_polygon;
		int last_mirror_index_1;
		int last_mirror_index_2;
};

