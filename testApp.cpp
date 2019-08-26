#include "testApp.h"



float fastCosine(float angle){

	float cos;

	angle += 1.57079632;
	if (angle >  3.14159265)
		angle -= 6.28318531;

	if (angle < 0)
	{
		cos = 1.27323954 * angle + 0.405284735 * angle * angle;
    
		if (cos < 0)
			cos = .225 * (cos *-cos - cos) + cos;
		else
			cos = .225 * (cos * cos - cos) + cos;
	}
	else
	{
		cos = 1.27323954 * angle - 0.405284735 * angle * angle;

		if (cos < 0)
			cos = .225 * (cos *-cos - cos) + cos;
		else
			cos = .225 * (cos * cos - cos) + cos;
	}
	return cos;

}

float fastSine(float angle){

	float sine;

	
	if (angle >  3.14159265)
		angle -= 6.28318531;

	if (angle < 0)
	{
		sine = 1.27323954 * angle + 0.405284735 * angle * angle;
    
		if (sine < 0)
			sine = .225 * (sine *-sine - sine) + sine;
		else
			sine = .225 * (sine * sine - sine) + sine;
	}
	else
	{
		sine = 1.27323954 * angle - 0.405284735 * angle * angle;

		if (sine < 0)
			sine = .225 * (sine *-sine - sine) + sine;
		else
			sine = .225 * (sine * sine - sine) + sine;
	}
	return sine;


}

bool intersection_compare(intersection_information i1, intersection_information i2){

	if (i1.projection_position < i2.projection_position){
		return false;
	}
	else return true;

}

//--------------------------------------------------------------
void testApp::setup(){

	ofBackground(0, 0, 0);

	playerpoint.x = 200;
	playerpoint.y = 200;

	last_time = 0.0f;

	player_movement_rate = 120.0f;
	player_turning_rate = 3.0f;
	

	//This box surrounds the play area, helps with lighting
	myPolygon bounding_box;

	myPoint corner_1;
	myPoint corner_2;
	myPoint corner_3;
	myPoint corner_4;

	int window_width = ofGetWindowWidth();
	int window_height = ofGetWindowHeight();

	corner_1.x = -50;
	corner_1.y = -50;

	corner_2.x = window_width + 50;
	corner_2.y = -50;

	corner_3.x = window_width + 50;
	corner_3.y = window_height + 50;

	corner_4.x = -50;
	corner_4.y = window_height + 50;

	bounding_box.pointlist.push_back(corner_1);
	bounding_box.pointlist.push_back(corner_2);
	bounding_box.pointlist.push_back(corner_3);
	bounding_box.pointlist.push_back(corner_4);

	
	bounding_box.grabbed = false;
	bounding_box.type = SOLID;

	polylist.push_back(bounding_box);


	myPolygon newpoly;

	newpoly.color.r = ofRandom(0, 255);
	newpoly.color.g = ofRandom(0, 255);
	newpoly.color.b = ofRandom(0, 255);

	newpoly.grabbed = false;
	newpoly.type = SOLID;

	polylist.push_back(newpoly);
	
	current_poly_index = 1;

	
	view_angle = MY_PI/6;

	view_angle_cos = fastCosine(view_angle);
	view_angle_sine = fastSine(view_angle);

	view_angle_tan = view_angle_sine / view_angle_cos;

	myColor white;
	white.r = 255;
	white.g = 255;
	white.b = 255;

	screen_bottom_corner.x = 50;
	screen_bottom_corner.y = 50;

	screen_width = 200;
	screen_height = 40;

	eye_radius = 25;
	pupil_depth = 0.3;
	sclera_depth = 0.4;
	
	for(int x = 0 ; x < 1024; x++){
		pressed[x] = false;
	}

	player_direction.x = 1;
	player_direction.y = 0;
	
	point_selected = false;

	light_placement = false;

	view_rays = false;

	last_mirror_polygon = -1;
	last_mirror_index_1 = -1;
	last_mirror_index_2 = -1;

}

//--------------------------------------------------------------
void testApp::update(){

	if(point_selected){
		
		polylist[polygon_selected_index].pointlist[point_selected_index].x = mouse_x;
		polylist[polygon_selected_index].pointlist[point_selected_index].y = mouse_y;
		
	}
	
	float time_diff = ofGetElapsedTimef() - last_time;

	if(time_diff < 0) cout<<"whoa!\n";

	last_time = ofGetElapsedTimef();
	

	
	myPoint new_player_direction;
	

	//TURNING WITH MOUSE
	new_player_direction.x = mouse_x - playerpoint.x;
	new_player_direction.y = mouse_y - playerpoint.y;
	

	float direction_distance = sqrt(new_player_direction.x * new_player_direction.x + new_player_direction.y * new_player_direction.y);
	new_player_direction.x /= direction_distance;
	new_player_direction.y /= direction_distance;
	
	myPoint player_rotation;

	player_rotation.x = new_player_direction.x * player_direction.x + new_player_direction.y * player_direction.y;
	player_rotation.y = new_player_direction.y * player_direction.x - new_player_direction.x * player_direction.y;

	/* TURNING WITH KEYS
	float cos;
	float sin;
	if(pressed['l']){
		 cos = fastCosine(player_turning_rate * time_diff);
		 sin = fastSine(player_turning_rate * time_diff);

		new_player_direction.x = player_direction.x *cos -player_direction.y * sin;
		new_player_direction.y = player_direction.x * sin + player_direction.y * cos;
	}
	else if(pressed['k']){
		 cos = fastCosine(-player_turning_rate * time_diff);
		 sin = fastSine(-player_turning_rate * time_diff);

		new_player_direction.x = player_direction.x *cos -player_direction.y * sin;
		new_player_direction.y = player_direction.x * sin + player_direction.y * cos;
	}
	else{

		cos = 1;
		sin = 0;


		new_player_direction.x = player_direction.x *cos -player_direction.x * sin;
		new_player_direction.y = player_direction.x * sin + player_direction.y * cos;
	}

	myPoint player_rotation;
	player_rotation.x = cos;
	player_rotation.y = sin;
	*/


	myPolygon grabbed_poly_new_position;
	bool poly_grabbed = false;
	int grabbed_poly_index;

	for(int k = 0 ; k < polylist.size(); k++){
		if ((polylist[k].grabbed) == true){

			poly_grabbed = true;
			grabbed_poly_index = k;

			//This is really stupid, surely there is a better way of doing this?
			grabbed_poly_new_position.pointlist.resize(polylist[k].pointlist.size());
			grabbed_poly_new_position.type = polylist[k].type;
			grabbed_poly_new_position.color = polylist[k].color;
			grabbed_poly_new_position.line_list = polylist[k].line_list;
			grabbed_poly_new_position.grabbed = true;

			for(int i = 0; i < polylist[k].pointlist.size(); i ++){

				myPoint player_to_point;
				myPoint rotated_player_to_point;
				player_to_point.x = playerpoint.x - polylist[k].pointlist[i].x;
				player_to_point.y = playerpoint.y - polylist[k].pointlist[i].y;

				rotated_player_to_point.x = player_to_point.x * player_rotation.x - player_to_point.y * player_rotation.y;
				rotated_player_to_point.y = player_to_point.x * player_rotation.y + player_to_point.y * player_rotation.x;

				grabbed_poly_new_position.pointlist[i].x = playerpoint.x - rotated_player_to_point.x;
				grabbed_poly_new_position.pointlist[i].y = playerpoint.y - rotated_player_to_point.y;

			}
			break;
		}
	}
	

	float player_displacement_x  = 0;
	float player_displacement_y  = 0;

	if(pressed['w']){
		player_displacement_x +=  player_direction.x * time_diff * player_movement_rate;
		player_displacement_y += player_direction.y * time_diff * player_movement_rate;
	}
	if (pressed['s']){
		player_displacement_x -= player_direction.x * time_diff * player_movement_rate;
		player_displacement_y -= player_direction.y * time_diff * player_movement_rate;
	}
	if (pressed['a']){
		
		player_displacement_x += player_direction.y * time_diff*player_movement_rate;
		player_displacement_y -= player_direction.x * time_diff*player_movement_rate;
	}
	if (pressed['d']){
		player_displacement_x -= player_direction.y * time_diff*player_movement_rate;
		player_displacement_y += player_direction.x * time_diff*player_movement_rate;
	}


	if(poly_grabbed){
		for(int i = 0; i < grabbed_poly_new_position.pointlist.size(); i++){
			grabbed_poly_new_position.pointlist[i].x += player_displacement_x;
			grabbed_poly_new_position.pointlist[i].y += player_displacement_y;
		}

		bool intersects = false;
		for(int k = 0; k < polylist.size(); k++){

			if(k == grabbed_poly_index) continue;

			//if(polygonsIntersect(grabbed_poly_new_position, polylist[k])){
			//	intersects = true;
			//	break;
			//}
			
		}
		if(!intersects){
			polylist[grabbed_poly_index] = grabbed_poly_new_position;
			player_direction = new_player_direction;
			playerpoint.x += player_displacement_x;
			playerpoint.y += player_displacement_y;

		}
		else{
			//do not update player or polygon positions


		}
	}
	else{

		player_direction = new_player_direction;
		playerpoint.x += player_displacement_x;
		playerpoint.y += player_displacement_y;

	}
	
	

}

//--------------------------------------------------------------
void testApp::draw(){

	//Update polygon orientations for lighting
	Update_Polygon_Orientations();

	//Before figuring out what the player can see, we illuminate stuff with the lights

	//THe line list should always be the same size as the point list

	//Reset all line colours
	for(int i = 0; i < polylist.size(); i++){
		polylist[i].line_list.resize(polylist[i].pointlist.size());

		for(int j = 0; j < polylist[i].line_list.size(); j++){

			polylist[i].line_list[j].dividers.clear();
			polylist[i].line_list[j].dividers.push_back(0);

			polylist[i].line_list[j].colors.clear();
			polylist[i].line_list[j].colors.push_back(polylist[i].color);

		}
	}
	
	ofFill();
	ofEnableAlphaBlending();
	ofSetColor(255,0,0);
	for(int i = 0; i < light_list.size(); i++){

		vector<point_info> point_list;
		//Sort the points by their angle with the light
		for(int k = 0; k < polylist.size(); k++){

			if ( polylist[k].pointlist.size() < 2) continue;

			for(int j = 0; j < polylist[k].pointlist.size(); j++){

				myPoint light_to_point;
				light_to_point.x = polylist[k].pointlist[j].x - light_list[i].position.x;
				light_to_point.y = polylist[k].pointlist[j].y - light_list[i].position.y;

				//What is the angle of this line?
				float light_to_point_magnitude = sqrt(light_to_point.x * light_to_point.x + light_to_point.y * light_to_point.y);
				float light_to_point_cos = light_to_point.x / light_to_point_magnitude;
				float light_to_point_sin = light_to_point.y / light_to_point_magnitude;

				float light_to_point_angle = acos(light_to_point_cos);
				if(light_to_point_sin < 0){
					light_to_point_angle = 2 * MY_PI - light_to_point_angle;
				}

				point_info this_point_info;
				this_point_info.point_iter = j;
				this_point_info.poly_iter = k;
				this_point_info.angle = light_to_point_angle;


				//Sort the points into the list as they arrive
				if (point_list.size() == 0){
					point_list.push_back(this_point_info);
				}
				else{
					point_list.resize(point_list.size() + 1);
					for(int q = 0; q < point_list.size(); q++){

						if(q == point_list.size() - 1){
							point_list[q] = this_point_info;
							break;
						}


						if (point_list[q].angle > this_point_info.angle){
							for(int x = point_list.size() - 1; x > q; x--){
								point_list[x] = point_list[x-1];
							}
							point_list[q] = this_point_info;
							break;
						}

					}
				}

			}


		}
		
		ofSetColor(255,0,0);

		if (point_list.size() < 2) continue;

		int list_size = point_list.size();

		vector<intersection_information> info_list;

		for(int k = 0; k < list_size; k++){

			myPoint light_to_point;
			light_to_point.x = polylist[point_list[k].poly_iter].pointlist[point_list[k].point_iter].x - light_list[i].position.x;
			light_to_point.y = polylist[point_list[k].poly_iter].pointlist[point_list[k].point_iter].y - light_list[i].position.y;

			info_list.push_back(Closest_Intersection(light_list[i].position, light_to_point,0,0,false,point_list[k].poly_iter, point_list[k].point_iter));

		}

		//Now we sweep clockwise through the points
		int current_polygon_num;
		int current_edge_num;
		float current_block_start;
		float current_block_end;



		bool finished = false;

		for(int k = 0; k < list_size + 1; k++){

			

			if ( k == 0){

				if ( info_list[0].passed_target == true){

					if(info_list[1].polygon_num == info_list[0].passed_polygon_num ||
						(info_list[1].passed_target == true && info_list[1].passed_polygon_num == info_list[0].passed_polygon_num)){

							current_polygon_num = info_list[0].passed_polygon_num;
							if ( polylist[info_list[0].passed_polygon_num].orientation == CLOCKWISE){
								current_edge_num = info_list[0].passed_point_num - 1;
								if(current_edge_num == -1){
									current_edge_num = polylist[info_list[0].polygon_num].pointlist.size() - 1;
								}
								current_block_start = 1;
							}
							else if (polylist[info_list[0].passed_polygon_num].orientation == COUNTERCLOCKWISE){
								current_edge_num = info_list[0].passed_point_num;
								current_block_start = 0;

							}


					}
					else{

						current_polygon_num = info_list[0].polygon_num;
						current_edge_num = info_list[0].edge_num;
						current_block_start = info_list[0].edge_distance;

					}


				}
				else if ( info_list[0].hit_target == true){

					current_polygon_num = info_list[0].polygon_num;

					if (polylist[info_list[0].polygon_num].orientation == COUNTERCLOCKWISE){
						current_edge_num = info_list[0].edge_num;
						current_block_start = 0;
					}
					else{
						current_edge_num = info_list[0].edge_num - 1;
						if (current_edge_num == -1){
							current_edge_num = polylist[info_list[0].polygon_num].pointlist.size() - 1;
						}
						current_block_start = 1;
					}

				}
				else{
					current_polygon_num = info_list[0].polygon_num;
					current_edge_num = info_list[0].edge_num;
					current_block_start = info_list[0].edge_distance;
				}
				continue;
			}


			if ( k == list_size){

				k = 0; //Loop around to the start at the end
				finished = true;
			}

			int k_plus_one;
			if (k == list_size - 1){
				k_plus_one = 0;
			}
			else{
				k_plus_one = k+1;
			}

			if ( info_list[k].passed_target == true){

				//We have reached the end of the current edge we are sweeping
				if (info_list[k].passed_polygon_num == current_polygon_num &&
					(info_list[k].polygon_num != current_polygon_num ||
					info_list[k].edge_num != current_edge_num)){

					if ( info_list[k].passed_point_num == current_edge_num){

						current_block_end = 0;

					}
					else{
						current_block_end = 1;
					}
					//Draw the line
					Color_Line_Segment(current_polygon_num, current_edge_num, current_block_start, current_block_end, light_list[i].color);
					
					myPoint edge_direction;

					int current_edge_num_plus_one;

					if(current_polygon_num < 0 || current_polygon_num > polylist.size()){
						//This should never happen
						int x = 5;

				 	}

					if (current_edge_num == polylist[current_polygon_num].pointlist.size() - 1){
						current_edge_num_plus_one = 0;
					}
					else{
						current_edge_num_plus_one = current_edge_num + 1;
					}


					edge_direction.x = polylist[current_polygon_num].pointlist[current_edge_num_plus_one].x - 
						polylist[current_polygon_num].pointlist[current_edge_num].x;
					
					edge_direction.y = polylist[current_polygon_num].pointlist[current_edge_num_plus_one].y - 
						polylist[current_polygon_num].pointlist[current_edge_num].y;
					
					//And draw the triangle
					float x1 = light_list[i].position.x;
					float y1 = light_list[i].position.y;

					float x2 = polylist[current_polygon_num].pointlist[current_edge_num].x + edge_direction.x * current_block_start;
					float y2 = polylist[current_polygon_num].pointlist[current_edge_num].y + edge_direction.y * current_block_start;


					float x3 = polylist[current_polygon_num].pointlist[current_edge_num].x + edge_direction.x * current_block_end;
					float y3 = polylist[current_polygon_num].pointlist[current_edge_num].y + edge_direction.y * current_block_end;

					ofSetColor(light_list[i].color.r, light_list[i].color.g, light_list[i].color.b, 127);
					ofTriangle(x1, y1, x2, y2, x3, y3);

					current_block_start = info_list[k].edge_distance;
					current_polygon_num = info_list[k].polygon_num;
					current_edge_num  = info_list[k].edge_num;
				}

				else{

					//We have again reached the end of the current edge we are sweeping, but in a different way
					current_block_end = info_list[k].edge_distance;
					Color_Line_Segment(current_polygon_num, current_edge_num, current_block_start, current_block_end, light_list[i].color);

					myPoint edge_direction;

					int current_edge_num_plus_one;
					if (current_edge_num == polylist[current_polygon_num].pointlist.size() - 1){
						current_edge_num_plus_one = 0;
					}
					else{
						current_edge_num_plus_one = current_edge_num + 1;
					}


					edge_direction.x = polylist[current_polygon_num].pointlist[current_edge_num_plus_one].x - 
						polylist[current_polygon_num].pointlist[current_edge_num].x;
					
					edge_direction.y = polylist[current_polygon_num].pointlist[current_edge_num_plus_one].y - 
						polylist[current_polygon_num].pointlist[current_edge_num].y;
					
					//And draw the triangle
					float x1 = light_list[i].position.x;
					float y1 = light_list[i].position.y;

					float x2 = polylist[current_polygon_num].pointlist[current_edge_num].x + edge_direction.x * current_block_start;
					float y2 = polylist[current_polygon_num].pointlist[current_edge_num].y + edge_direction.y * current_block_start;


					float x3 = polylist[current_polygon_num].pointlist[current_edge_num].x + edge_direction.x * current_block_end;
					float y3 = polylist[current_polygon_num].pointlist[current_edge_num].y + edge_direction.y * current_block_end;

					ofSetColor(light_list[i].color.r, light_list[i].color.g, light_list[i].color.b, 127);
					ofTriangle(x1, y1, x2, y2, x3, y3);

					int passed_point_num_plus_one;
					if ( info_list[k].passed_point_num == polylist[info_list[k].passed_polygon_num].pointlist.size() - 1){

						passed_point_num_plus_one = 0;
					}
					else{
						passed_point_num_plus_one = info_list[k].passed_point_num + 1;
					}

					if(info_list[k_plus_one].edge_num == info_list[k].passed_point_num ||
						info_list[k_plus_one].edge_num == passed_point_num_plus_one){
						current_block_start = 0;
						current_polygon_num = info_list[k].passed_polygon_num;
						current_edge_num  = info_list[k].passed_point_num;
					}
					else{
						current_block_start = 1;
						current_polygon_num = info_list[k].passed_polygon_num;
						
						if ( info_list[k].passed_point_num == 0){
							current_edge_num = polylist[current_polygon_num].pointlist.size() - 1;
						}
						else{
							current_edge_num  = info_list[k].passed_point_num - 1;
						}

					}
				}

			}

			//We are moving from one edge to another on the same polygon
			else if (info_list[k].hit_target == true){

				if ( info_list[k].edge_num == current_edge_num){

					current_block_end = 0;
					
					
					Color_Line_Segment(current_polygon_num, current_edge_num, current_block_start, current_block_end, light_list[i].color);
					myPoint edge_direction;

					int current_edge_num_plus_one;
					if (current_edge_num == polylist[current_polygon_num].pointlist.size() - 1){
						current_edge_num_plus_one = 0;
					}
					else{
						current_edge_num_plus_one = current_edge_num + 1;
					}


					edge_direction.x = polylist[current_polygon_num].pointlist[current_edge_num_plus_one].x - 
						polylist[current_polygon_num].pointlist[current_edge_num].x;
					
					edge_direction.y = polylist[current_polygon_num].pointlist[current_edge_num_plus_one].y - 
						polylist[current_polygon_num].pointlist[current_edge_num].y;
					
					//And draw the triangle
					float x1 = light_list[i].position.x;
					float y1 = light_list[i].position.y;

					float x2 = polylist[current_polygon_num].pointlist[current_edge_num].x + edge_direction.x * current_block_start;
					float y2 = polylist[current_polygon_num].pointlist[current_edge_num].y + edge_direction.y * current_block_start;


					float x3 = polylist[current_polygon_num].pointlist[current_edge_num].x + edge_direction.x * current_block_end;
					float y3 = polylist[current_polygon_num].pointlist[current_edge_num].y + edge_direction.y * current_block_end;

					ofSetColor(light_list[i].color.r, light_list[i].color.g, light_list[i].color.b, 127);
					ofTriangle(x1, y1, x2, y2, x3, y3);
					
					current_block_start = 1;
					if (current_edge_num == 0){
						current_edge_num = polylist[current_polygon_num].pointlist.size() - 1;
					}
					else{
						current_edge_num -= 1;
					}
				}
				else{
					current_block_end = 1;
					Color_Line_Segment(current_polygon_num, current_edge_num, current_block_start, current_block_end, light_list[i].color);
					
					myPoint edge_direction;

					int current_edge_num_plus_one;

					if(current_polygon_num < 0 || current_polygon_num > polylist.size()){
						//This should never happen
						int x = 5;

					}

					if (current_edge_num == polylist[current_polygon_num].pointlist.size() - 1){
						current_edge_num_plus_one = 0;
					}
					else{
						current_edge_num_plus_one = current_edge_num + 1;
					}


					edge_direction.x = polylist[current_polygon_num].pointlist[current_edge_num_plus_one].x - 
						polylist[current_polygon_num].pointlist[current_edge_num].x;
					
					edge_direction.y = polylist[current_polygon_num].pointlist[current_edge_num_plus_one].y - 
						polylist[current_polygon_num].pointlist[current_edge_num].y;
					
					//And draw the triangle
					float x1 = light_list[i].position.x;
					float y1 = light_list[i].position.y;

					float x2 = polylist[current_polygon_num].pointlist[current_edge_num].x + edge_direction.x * current_block_start;
					float y2 = polylist[current_polygon_num].pointlist[current_edge_num].y + edge_direction.y * current_block_start;


					float x3 = polylist[current_polygon_num].pointlist[current_edge_num].x + edge_direction.x * current_block_end;
					float y3 = polylist[current_polygon_num].pointlist[current_edge_num].y + edge_direction.y * current_block_end;

					ofSetColor(light_list[i].color.r, light_list[i].color.g, light_list[i].color.b, 127);
					ofTriangle(x1, y1, x2, y2, x3, y3);
					
					current_block_start = 0;
					if(current_edge_num == polylist[current_polygon_num].pointlist.size() - 1){
						current_edge_num = 0;
					}
					else{
						current_edge_num += 1;
					}
				}

			}
			else{
				//Otherwise, we are still on the same edge
				//So...just do nothing?
				if(finished){

					current_block_end = info_list[k].edge_distance;
					Color_Line_Segment(current_polygon_num, current_edge_num, current_block_start, current_block_end, light_list[i].color);

					myPoint edge_direction;

					int current_edge_num_plus_one;
					if (current_edge_num == polylist[current_polygon_num].pointlist.size() - 1){
						current_edge_num_plus_one = 0;
					}
					else{
						current_edge_num_plus_one = current_edge_num + 1;
					}


					edge_direction.x = polylist[current_polygon_num].pointlist[current_edge_num_plus_one].x - 
						polylist[current_polygon_num].pointlist[current_edge_num].x;
					
					edge_direction.y = polylist[current_polygon_num].pointlist[current_edge_num_plus_one].y - 
						polylist[current_polygon_num].pointlist[current_edge_num].y;
					
					//And draw the triangle
					float x1 = light_list[i].position.x;
					float y1 = light_list[i].position.y;

					float x2 = polylist[current_polygon_num].pointlist[current_edge_num].x + edge_direction.x * current_block_start;
					float y2 = polylist[current_polygon_num].pointlist[current_edge_num].y + edge_direction.y * current_block_start;


					float x3 = polylist[current_polygon_num].pointlist[current_edge_num].x + edge_direction.x * current_block_end;
					float y3 = polylist[current_polygon_num].pointlist[current_edge_num].y + edge_direction.y * current_block_end;

					ofSetColor(light_list[i].color.r, light_list[i].color.g, light_list[i].color.b, 127);
					ofTriangle(x1, y1, x2, y2, x3, y3);

				}
				
			}
			if(finished) break;
		}

		

	}

	ofSetLineWidth(6);

	///We skip i == 1 because that is the bounding box
	for(int i = 1; i < polylist.size(); i++){

		draw_Polygon(polylist[i]);

		for(int k = 0; k < polylist[i].pointlist.size(); k++){

			myPoint start_point;
			myPoint end_point;

			if( k < polylist[i].pointlist.size() - 1){
				start_point = polylist[i].pointlist[k];
				end_point = polylist[i].pointlist[k+1];
			}
			else{
				start_point = polylist[i].pointlist[k];
				end_point = polylist[i].pointlist[0];
			}

			myPoint start_to_end;
			start_to_end.x = end_point.x - start_point.x;
			start_to_end.y = end_point.y - start_point.y;


			for(int j = 0; j < polylist[i].line_list[k].dividers.size(); j++){

				ofSetColor(polylist[i].line_list[k].colors[j].r, polylist[i].line_list[k].colors[j].g, polylist[i].line_list[k].colors[j].b);

				if (j == polylist[i].line_list[k].dividers.size() - 1){
					
					ofLine(start_point.x + start_to_end.x * polylist[i].line_list[k].dividers[j], start_point.y + start_to_end.y * polylist[i].line_list[k].dividers[j],
						   end_point.x, end_point.y);


				}
				else{

					ofLine(start_point.x + start_to_end.x * polylist[i].line_list[k].dividers[j], start_point.y + start_to_end.y * polylist[i].line_list[k].dividers[j],
						   start_point.x + start_to_end.x * polylist[i].line_list[k].dividers[j+1], start_point.y + start_to_end.y * polylist[i].line_list[k].dividers[j+1]);

				}
			}

		}

	}

	ofFill();

	ofSetColor(50, 50, 50);
	ofCircle(playerpoint.x, playerpoint.y, 2);

	ofSetColor(75, 75, 75);
	ofCircle(playerpoint.x, playerpoint.y, eye_radius);

	
	float mouse_angle = acos(player_direction.x);
	{
		ofSetColor(255, 255, 255);
		ofFill();

		ofColor white;
		white.r = 255;
		white.g = 255;
		white.b = 255;
	
		ofPath sclera; //The sclera is the white part of the eyeball
		ofPath sclera_2;
		sclera.setColor(white);
		sclera_2.setColor(white);

		//We draw the sclera in the intersection of two circles. Now we determine where the center of the second one is.
		myPoint circ_1;
		myPoint circ_2;
		myPoint circ_3;
	
		myPoint circ_center;

		myPoint circ_1_to_2;
		myPoint perpindicular;
		myPoint midpoint;

		circ_1.x = playerpoint.x + player_direction.x * eye_radius* (1 - sclera_depth);
		circ_1.y = playerpoint.y + player_direction.y * eye_radius * (1 - sclera_depth);


		float cos = fastCosine(view_angle);
		float sine = fastSine(view_angle);

		circ_2.x = playerpoint.x + (player_direction.x * eye_radius * cos - player_direction.y * eye_radius * sine);
		circ_2.y = playerpoint.y + (player_direction.x * eye_radius * sine + player_direction.y * eye_radius * cos);


		circ_3.x = playerpoint.x + player_direction.x * eye_radius * cos + player_direction.y * eye_radius * sine;
		circ_3.y = playerpoint.y + -player_direction.x * eye_radius * sine + player_direction.y * eye_radius * cos;

		circ_1_to_2.x = circ_2.x - circ_1.x;
		circ_1_to_2.y = circ_2.y - circ_1.y;
	
		midpoint.x = (circ_2.x + circ_1.x)/2;
		midpoint.y = (circ_2.y + circ_1.y)/2;

		perpindicular.x = circ_1_to_2.y +midpoint.x;
		perpindicular.y = -circ_1_to_2.x + midpoint.y;

		ofLine(midpoint.x, midpoint.y, perpindicular.x, perpindicular.y);

		circ_center.x = 
						( (playerpoint.x * circ_1.y - playerpoint.y*circ_1.x) * (midpoint.x - perpindicular.x) - 
						(playerpoint.x - circ_1.x) * (midpoint.x * perpindicular.y - midpoint.y * perpindicular.x) ) /
						  ( (playerpoint.x - circ_1.x) * (midpoint.y - perpindicular.y) - 
						  (playerpoint.y - circ_1.y) * (midpoint.x - perpindicular.x));
					  
		circ_center.y = 
							( (playerpoint.x * circ_1.y - playerpoint.y*circ_1.x) * (midpoint.y - perpindicular.y) - 
							(playerpoint.y - circ_1.y) * (midpoint.x * perpindicular.y - midpoint.y * perpindicular.x) ) /
							  ( (playerpoint.x - circ_1.x) * (midpoint.y - perpindicular.y) - 
							  (playerpoint.y - circ_1.y) * (midpoint.x - perpindicular.x));
		
	
		float circ_radius = sqrt((circ_center.x - circ_1.x) * (circ_center.x - circ_1.x) +
									(circ_center.y - circ_1.y) * (circ_center.y- circ_1.y) );
		
		
	
		sclera.moveTo(playerpoint.x + eye_radius * (1 - sclera_depth/2) *  player_direction.x, 
					playerpoint.y + eye_radius * (1 - sclera_depth/2) *  player_direction.y);

		sclera_2.moveTo(playerpoint.x + eye_radius * (1 - sclera_depth/2) *  player_direction.x, 
					playerpoint.y + eye_radius * (1 - sclera_depth/2) *  player_direction.y);

		float c2_c3_distance = sqrt( (circ_2.x - circ_3.x) * (circ_2.x - circ_3.x) + (circ_2.y - circ_3.y) * (circ_2.y - circ_3.y));
		float delta_angle = asin( c2_c3_distance/2 / circ_radius);

		if (player_direction.y > 0){

			sclera.arc(playerpoint.x, playerpoint.y, eye_radius, eye_radius,  (mouse_angle - view_angle) * 180 / MY_PI, (mouse_angle + view_angle) * 180/MY_PI);
			sclera.draw();
			sclera_2.arc(circ_center.x, circ_center.y, circ_radius,circ_radius,
						((MY_PI + mouse_angle) - (delta_angle)) * 180 / MY_PI,
						((MY_PI + mouse_angle) + (delta_angle)) * 180 / MY_PI);
			sclera_2.draw();

		}
		else{
			sclera.arc(playerpoint.x, playerpoint.y, eye_radius, eye_radius, -(mouse_angle + view_angle) * 180/MY_PI, -(mouse_angle - view_angle) * 180 / MY_PI);
			sclera.draw();
			sclera_2.arc(circ_center.x, circ_center.y, circ_radius,circ_radius,
						((MY_PI - mouse_angle) - (delta_angle)) * 180 / MY_PI,
						((MY_PI -  mouse_angle) + (delta_angle)) * 180 / MY_PI
						);
			sclera_2.draw();

		}

	}

	

	{
		ofSetColor(255, 255, 255);
		ofFill();

		ofColor black;
		black.r = 0;
		black.g = 0;
		black.b = 0;
	
		ofPath pupil; //The pupil is the dark part of the eyeball
		ofPath pupil_2;
		pupil.setColor(black);
		pupil_2.setColor(black);

		//We draw the pupil in the intersection of two circles. Now we determine where the center of the second one is.
		myPoint circ_1;
		myPoint circ_2;
		myPoint circ_3;
	
		myPoint circ_center;

		myPoint circ_1_to_2;
		myPoint perpindicular;
		myPoint midpoint;

		circ_1.x = playerpoint.x + player_direction.x * eye_radius* (1 - pupil_depth);
		circ_1.y = playerpoint.y + player_direction.y * eye_radius * (1 - pupil_depth);
		 

		float cos = fastCosine(view_angle/2);
		float sine = fastSine(view_angle/2);

		circ_2.x = playerpoint.x + (player_direction.x * eye_radius * cos - player_direction.y * eye_radius * sine);
		circ_2.y = playerpoint.y + (player_direction.x * eye_radius * sine + player_direction.y * eye_radius * cos);


		circ_3.x = playerpoint.x + player_direction.x * eye_radius * cos + player_direction.y * eye_radius * sine;
		circ_3.y = playerpoint.y + -player_direction.x * eye_radius * sine + player_direction.y * eye_radius * cos;

		circ_1_to_2.x = circ_2.x - circ_1.x;
		circ_1_to_2.y = circ_2.y - circ_1.y;
	
		midpoint.x = (circ_2.x + circ_1.x)/2;
		midpoint.y = (circ_2.y + circ_1.y)/2;

		perpindicular.x = circ_1_to_2.y +midpoint.x;
		perpindicular.y = -circ_1_to_2.x + midpoint.y;

		ofLine(midpoint.x, midpoint.y, perpindicular.x, perpindicular.y);

		circ_center.x = 
						( (playerpoint.x * circ_1.y - playerpoint.y*circ_1.x) * (midpoint.x - perpindicular.x) - 
						(playerpoint.x - circ_1.x) * (midpoint.x * perpindicular.y - midpoint.y * perpindicular.x) ) /
						  ( (playerpoint.x - circ_1.x) * (midpoint.y - perpindicular.y) - 
						  (playerpoint.y - circ_1.y) * (midpoint.x - perpindicular.x));
					  
		circ_center.y = 
							( (playerpoint.x * circ_1.y - playerpoint.y*circ_1.x) * (midpoint.y - perpindicular.y) - 
							(playerpoint.y - circ_1.y) * (midpoint.x * perpindicular.y - midpoint.y * perpindicular.x) ) /
							  ( (playerpoint.x - circ_1.x) * (midpoint.y - perpindicular.y) - 
							  (playerpoint.y - circ_1.y) * (midpoint.x - perpindicular.x));
		
	
		float circ_radius = sqrt((circ_center.x - circ_1.x) * (circ_center.x - circ_1.x) +
									(circ_center.y - circ_1.y) * (circ_center.y- circ_1.y) );
		
		
	
		pupil.moveTo(playerpoint.x + eye_radius * (1 - pupil_depth/2) *  player_direction.x, 
					playerpoint.y + eye_radius * (1 - pupil_depth/2) *  player_direction.y);

		pupil_2.moveTo(playerpoint.x + eye_radius * (1 ) *  player_direction.x, 
					playerpoint.y + eye_radius * (1 ) *  player_direction.y);

		float c2_c3_distance = sqrt( (circ_2.x - circ_3.x) * (circ_2.x - circ_3.x) + (circ_2.y - circ_3.y) * (circ_2.y - circ_3.y));
		float delta_angle = asin( c2_c3_distance/2 / circ_radius);

		if (player_direction.y > 0){

			pupil.arc(playerpoint.x, playerpoint.y, eye_radius, eye_radius,  (mouse_angle - view_angle/2) * 180 / MY_PI, (mouse_angle + view_angle/2) * 180/MY_PI);
			pupil.draw();
			pupil_2.arc(circ_center.x, circ_center.y, circ_radius,circ_radius,
						((MY_PI + mouse_angle) - (delta_angle)) * 180 / MY_PI,
						((MY_PI + mouse_angle) + (delta_angle)) * 180 / MY_PI);
			pupil_2.draw();

		}
		else{
			pupil.arc(playerpoint.x, playerpoint.y, eye_radius, eye_radius, -(mouse_angle + view_angle/2) * 180/MY_PI, -(mouse_angle - view_angle/2) * 180 / MY_PI);
			pupil.draw();
			pupil_2.arc(circ_center.x, circ_center.y, circ_radius,circ_radius,
						((MY_PI - mouse_angle) - (delta_angle)) * 180 / MY_PI,
						((MY_PI -  mouse_angle) + (delta_angle)) * 180 / MY_PI
						);
			pupil_2.draw();

		}

	}

	//Detemine player-to-mouse angle for later purposes
	float player_to_mouse_angle = acos(player_direction.x);
	if(asin(player_direction.y) > 0){
		player_to_mouse_angle = 2 * MY_PI - player_to_mouse_angle;
	}

	//Reset the player viewpoint line
	player_viewpoint.dividers.clear();
	player_viewpoint.dividers.push_back(0);

	player_viewpoint.colors.clear();
	myColor white;
	white.r = 255;
	white.g = 255;
	white.b = 255;
	player_viewpoint.colors.push_back(white);


	//Now, determine the player's viewpoint, once again by sweeping through visible points clockwise
	{
		vector<point_info> point_list;


			for(int k = 0; k < polylist.size(); k++){

				if ( polylist[k].pointlist.size() < 2) continue;
				 
				for(int j = 0; j < polylist[k].pointlist.size(); j++){

					myPoint player_to_point;
					player_to_point.x = polylist[k].pointlist[j].x - playerpoint.x;
					player_to_point.y = polylist[k].pointlist[j].y - playerpoint.y;

					//What is the angle of this line?
					float player_to_point_magnitude = sqrt(player_to_point.x * player_to_point.x + player_to_point.y * player_to_point.y);
					float light_to_point_cos = player_to_point.x / player_to_point_magnitude;
					float light_to_point_sin = player_to_point.y / player_to_point_magnitude;
					//Stupid OF coordinates require negating sine

					float light_to_point_angle = acos(light_to_point_cos);
					if(-light_to_point_sin < 0){
						light_to_point_angle = 2 * MY_PI - light_to_point_angle;
					}

					//Only consider points within the player's view arc
					if ( (light_to_point_angle < player_to_mouse_angle + view_angle &&
						light_to_point_angle > player_to_mouse_angle - view_angle) ||
						(light_to_point_angle - 2 * MY_PI < player_to_mouse_angle + view_angle &&
						light_to_point_angle - 2 * MY_PI > player_to_mouse_angle - view_angle) ||
						(light_to_point_angle + 2 * MY_PI < player_to_mouse_angle + view_angle &&
						light_to_point_angle + 2 * MY_PI > player_to_mouse_angle - view_angle)){

							//Then the point is within the view arc

					}
					else{
						continue;
					}

					//This stuff relevant for projections later
					//BUT IT DOESN'T WORK, WHY??!
					player_to_point.x /= player_to_point_magnitude;
					player_to_point.y /= -player_to_point_magnitude;
					myPoint rotated_player_to_point;
					rotated_player_to_point.x = player_to_point.x * player_direction.x + player_to_point.y * player_direction.y;
					rotated_player_to_point.y = player_to_point.y * player_direction.x - player_to_point.x * player_direction.y;

					float tan = rotated_player_to_point.y / rotated_player_to_point.x;

					point_info this_point_info;
					this_point_info.point_iter = j;
					this_point_info.poly_iter = k;
					this_point_info.angle = light_to_point_angle;
					this_point_info.tan = tan;


					//Sort the points into the list as they arrive
					if (point_list.size() == 0){
						point_list.push_back(this_point_info);
					 }
					else{
						point_list.resize(point_list.size() + 1);
						for(int q = 0; q < point_list.size(); q++){

							if(q == point_list.size() - 1){
								point_list[q] = this_point_info;
								break;
							}


							if (point_list[q].angle > this_point_info.angle){
								for(int x = point_list.size() - 1; x > q; x--){
									point_list[x] = point_list[x-1];
								}
								point_list[q] = this_point_info;
								break;
							}

						}
					}

				}


			}
		
			ofSetColor(255,0,0);

			int list_size = point_list.size();

			vector<intersection_information> info_list;

			//The bounding edges of vision must be added to the list before and after normal processing
			myPoint vision_upper_arc;
			myPoint vision_lower_arc;

			vision_upper_arc.x = player_direction.x * view_angle_cos - player_direction.y * view_angle_sine;
			vision_upper_arc.y = player_direction.x * view_angle_sine + player_direction.y * view_angle_cos;

			vision_lower_arc.x = player_direction.x * view_angle_cos + player_direction.y * view_angle_sine;
			vision_lower_arc.y = - player_direction.x * view_angle_sine + player_direction.y * view_angle_cos;

			//Upper edge
			info_list.push_back(Closest_Intersection(playerpoint, vision_upper_arc, 0,0,false,-1,-1));


			for(int k = 0; k < list_size; k++){

				myPoint player_to_point;
				player_to_point.x = polylist[point_list[k].poly_iter].pointlist[point_list[k].point_iter].x - playerpoint.x;
				player_to_point.y = polylist[point_list[k].poly_iter].pointlist[point_list[k].point_iter].y - playerpoint.y;

				info_list.push_back(Closest_Intersection(playerpoint, player_to_point,0,0,false,point_list[k].poly_iter, point_list[k].point_iter));

			}

			//Lower edge
			info_list.push_back(Closest_Intersection(playerpoint, vision_lower_arc, 0,0, false, -1,-1));

			//Now we sweep clockwise through the points
			int current_polygon_num;
			int current_edge_num;
			float current_block_start;
			float current_block_end;

			float current_projection_start = 0;
			float current_projection_end;



			bool finished = false;

			for(int k = 0; k < info_list.size() ; k++){

				float current_iter_tan;
				if(k == 0){
					current_iter_tan = -view_angle_tan;
				}
				else if (k == info_list.size() - 1){
					current_iter_tan = view_angle_tan;
				}
				else{
					current_iter_tan = point_list[k-1].tan;
				}

				if ( k == 0){

					current_polygon_num = info_list[0].polygon_num;
					current_edge_num = info_list[0].edge_num;
					current_block_start = info_list[0].edge_distance;
					
					continue;
				}


				if ( k == list_size - 1){

					finished = true;

				}

				int k_plus_one;
				if (k == list_size - 1){
					k_plus_one = 0;
				}
				else{
					k_plus_one = k+1;
				}

				if ( info_list[k].passed_target == true){

					//We have reached the end of the current edge we are sweeping
					if (info_list[k].passed_polygon_num == current_polygon_num &&
						(info_list[k].polygon_num != current_polygon_num ||
						info_list[k].edge_num != current_edge_num)){

						if ( info_list[k].passed_point_num == current_edge_num){

							current_block_end = 0;

						}
						else{
							current_block_end = 1;
						}

						current_projection_end = 0.5 +  (current_iter_tan / view_angle_tan) / 2;
						

						Add_View_Interval(current_polygon_num, current_edge_num, current_block_start, current_block_end,
							current_projection_start, current_projection_end);

						current_projection_start = current_projection_end;
						
						current_block_start = info_list[k].edge_distance;
						current_polygon_num = info_list[k].polygon_num;
						current_edge_num  = info_list[k].edge_num;

						

						
					}

					else{

						//We have again reached the end of the current edge we are sweeping, but in a different way
						current_block_end = info_list[k].edge_distance;
						
						current_projection_end = 0.5 +  (current_iter_tan / view_angle_tan) / 2;

						Add_View_Interval(current_polygon_num, current_edge_num, current_block_start, current_block_end,
							current_projection_start, current_projection_end);

						current_projection_start = current_projection_end;

						myPoint edge_direction;

						int current_edge_num_plus_one;
						if (current_edge_num == polylist[current_polygon_num].pointlist.size() - 1){
							current_edge_num_plus_one = 0;
						}
						else{
							current_edge_num_plus_one = current_edge_num + 1;
						}


						edge_direction.x = polylist[current_polygon_num].pointlist[current_edge_num_plus_one].x - 
							polylist[current_polygon_num].pointlist[current_edge_num].x;
					
						edge_direction.y = polylist[current_polygon_num].pointlist[current_edge_num_plus_one].y - 
							polylist[current_polygon_num].pointlist[current_edge_num].y;
					

						int passed_point_num_plus_one;
						if ( info_list[k].passed_point_num == polylist[info_list[k].passed_polygon_num].pointlist.size() - 1){

							passed_point_num_plus_one = 0;
						}
						else{
							passed_point_num_plus_one = info_list[k].passed_point_num + 1;
						}

						if(info_list[k_plus_one].edge_num == info_list[k].passed_point_num ||
							info_list[k_plus_one].edge_num == passed_point_num_plus_one){
							current_block_start = 0;
							current_polygon_num = info_list[k].passed_polygon_num;
							current_edge_num  = info_list[k].passed_point_num;
						}
						else{
							current_block_start = 1;
							current_polygon_num = info_list[k].passed_polygon_num;
						
							if ( info_list[k].passed_point_num == 0){
								current_edge_num = polylist[current_polygon_num].pointlist.size() - 1;
							}
							else{
								current_edge_num  = info_list[k].passed_point_num - 1;
							}

						}
					}

				}

				//We are moving from one edge to another on the same polygon
				else if (info_list[k].hit_target == true){

					if ( info_list[k].edge_num == current_edge_num){

						current_block_end = 0;

						current_projection_end = 0.5 +  (current_iter_tan / view_angle_tan) / 2;
						

						Add_View_Interval(current_polygon_num, current_edge_num, current_block_start, current_block_end,
							current_projection_start, current_projection_end);

						current_projection_start = current_projection_end;
					
						myPoint edge_direction;

						int current_edge_num_plus_one;
						if (current_edge_num == polylist[current_polygon_num].pointlist.size() - 1){
							current_edge_num_plus_one = 0;
						}
						else{
							current_edge_num_plus_one = current_edge_num + 1;
						}


						edge_direction.x = polylist[current_polygon_num].pointlist[current_edge_num_plus_one].x - 
							polylist[current_polygon_num].pointlist[current_edge_num].x;
					
						edge_direction.y = polylist[current_polygon_num].pointlist[current_edge_num_plus_one].y - 
							polylist[current_polygon_num].pointlist[current_edge_num].y;
					
					
						current_block_start = 1;
						if (current_edge_num == 0){
							current_edge_num = polylist[current_polygon_num].pointlist.size() - 1;
						}
						else{
							current_edge_num -= 1;
						}
					}
					else{
						current_block_end = 1;

						current_projection_end = 0.5 +  (current_iter_tan / view_angle_tan) / 2;
						

						Add_View_Interval(current_polygon_num, current_edge_num, current_block_start, current_block_end,
							current_projection_start, current_projection_end);

						current_projection_start = current_projection_end;
					
						myPoint edge_direction;

						int current_edge_num_plus_one;

						if(current_polygon_num < 0 || current_polygon_num > polylist.size()){
							//This should never happen
							int x = 5;

						}

						if (current_edge_num == polylist[current_polygon_num].pointlist.size() - 1){
							current_edge_num_plus_one = 0;
						}
						else{
							current_edge_num_plus_one = current_edge_num + 1;
						}


						edge_direction.x = polylist[current_polygon_num].pointlist[current_edge_num_plus_one].x - 
							polylist[current_polygon_num].pointlist[current_edge_num].x;
					
						edge_direction.y = polylist[current_polygon_num].pointlist[current_edge_num_plus_one].y - 
							polylist[current_polygon_num].pointlist[current_edge_num].y;
										
						current_block_start = 0;
						if(current_edge_num == polylist[current_polygon_num].pointlist.size() - 1){
							current_edge_num = 0;
						}
						else{
							current_edge_num += 1;
						}
					}

				}
				else{
					//Otherwise, we are still on the same edge
					//So...just do nothing?
					if(finished){

						current_block_end = info_list[k].edge_distance;

						current_projection_end = 0.5 +  (current_iter_tan / view_angle_tan) / 2;
						

						Add_View_Interval(current_polygon_num, current_edge_num, current_block_start, current_block_end,
							current_projection_start, current_projection_end);

						current_projection_start = current_projection_end;

						myPoint edge_direction;

						int current_edge_num_plus_one;
						if (current_edge_num == polylist[current_polygon_num].pointlist.size() - 1){
							current_edge_num_plus_one = 0;
						}
						else{
							current_edge_num_plus_one = current_edge_num + 1;
						}


						edge_direction.x = polylist[current_polygon_num].pointlist[current_edge_num_plus_one].x - 
							polylist[current_polygon_num].pointlist[current_edge_num].x;
					
						edge_direction.y = polylist[current_polygon_num].pointlist[current_edge_num_plus_one].y - 
							polylist[current_polygon_num].pointlist[current_edge_num].y;
					
					}
				
				}
				if(finished) break;
			}
	}


	ofFill();
	//Draw the player viewpoint;
	for(int i = 0; i < player_viewpoint.dividers.size(); i++){

		float x1;
		x1 = player_viewpoint.dividers[i] * screen_width;
		x1 += screen_bottom_corner.x;

		float x2;
		if( i < player_viewpoint.dividers.size() - 1){
			x2 = player_viewpoint.dividers[i+1] * screen_width;
			x2 += screen_bottom_corner.x;
		}
		else{
			x2 = screen_width + screen_bottom_corner.x;
		}

		ofSetColor(player_viewpoint.colors[i].r, player_viewpoint.colors[i].g, player_viewpoint.colors[i].b);
		ofRect(x1, screen_bottom_corner.y, x2 - x1, screen_height);

	}

	ofSetLineWidth(2);

	//Bounding box
	ofNoFill();
	ofSetColor(255,0,0);
	ofRect(screen_bottom_corner.x, screen_bottom_corner.y, screen_width, screen_height);
	
	//Does the polygon that the player is carrying overlap with any other polygons?
	for(int k = 0; k < polylist.size(); k++){

		if (polylist[k].grabbed == true){

			bool overlaps = false;

			//Skip checking against the bounding box
			for(int j = 1; j < polylist.size(); j++){

				if(j == k) continue;

				if(polylist[j].pointlist.size() > 1 &&polygonsIntersect(polylist[k], polylist[j])){

					overlaps = true;
					break;

				}
			}

			if(overlaps) ofSetColor(255, 0, 0);
			else ofSetColor(0,255,0);

			ofCircle(250, 250, 10);

		}

	}
}

//--------------------------------------------------------------
void testApp::keyPressed(int key){
	current_key_pressed = key;
	pressed[key] = true;
}

//--------------------------------------------------------------
void testApp::keyReleased(int key){
	current_key_pressed = NO_KEY_PRESSED;
	pressed[key] = false;

	if(key == 'n'){
		current_poly_index++;
		myPolygon newpoly;

		newpoly.color.r = ofRandom(0, 255);
		newpoly.color.g = ofRandom(0, 255);
		newpoly.color.b = ofRandom(0, 255);

		newpoly.grabbed = false;

		newpoly.type = SOLID;

		polylist.push_back(newpoly);
	}

	if(key == 'l'){

		light_placement = true;
	}

	if (key == 't'){
		t=true;
	}
	
	if(key == 'm'){
		current_poly_index++;
		myPolygon newpoly;

		newpoly.color.r = 255;
		newpoly.color.g = 255;
		newpoly.color.b = 255;

		newpoly.grabbed = false;

		newpoly.type = MIRROR;

		polylist.push_back(newpoly);

	}

	if(key == SPACE_BAR){

		bool none_grabbed = true;

		for(int k = 0; k < polylist.size(); k++){
			if((polylist[k].grabbed) == true){

				polylist[k].grabbed = false;
				none_grabbed = false;

			}

		}

		if(none_grabbed){

			intersection_information gaze_intersection;
			gaze_intersection = Closest_Intersection(playerpoint, player_direction, 0, 0, false, -1, -1);

			if(gaze_intersection.distance < 50){

				polylist[gaze_intersection.polygon_num].grabbed = true;


			}

		}
	}
	if(key == 'v'){

		view_rays = !view_rays;
	}

	if (key == 't'){
		for(int i = 0; i < view_screen.size(); i++){
			screen_shot[i] = view_screen[i];
		}
	}

	cout<<key<<"\n";
}

//--------------------------------------------------------------
void testApp::mouseMoved(int x, int y){
	mouse_x = x;
	mouse_y = y;
}

//--------------------------------------------------------------
void testApp::mouseDragged(int x, int y, int button){
	mouse_x = x;
	mouse_y = y;
}

//--------------------------------------------------------------
void testApp::mousePressed(int x, int y, int button){
	
	//Is the click on the vertex of a polygon?
	for(int k = 0; k < polylist.size(); k++){

		if(polylist[k].pointlist.size() == 0) continue;

		for(int i = 0 ; i < polylist[k].pointlist.size(); i++){

			if (( x - polylist[k].pointlist[i].x ) * ( x - polylist[k].pointlist[i].x ) +  ( y - polylist[k].pointlist[i].y ) * ( y - polylist[k].pointlist[i].y ) < 36){
				
				point_selected = true;
				polygon_selected_index = k;
				point_selected_index = i;
				
				original_selected_position.x = polylist[k].pointlist[i].x;
				original_selected_position.y = polylist[k].pointlist[i].y;

				return;
			}

		}
	}

	//Is the click on the line of a polygon?
	for(int k = 0; k < polylist.size(); k++){

		if(polylist[k].pointlist.size() < 2) continue;

		for(int i = 0 ; i < polylist[k].pointlist.size() - 1; i++){
			
			//Find the distance between the point and the line
			float x1 = polylist[k].pointlist[i].x;
			float y1 = polylist[k].pointlist[i].y;
			
			float x2 = polylist[k].pointlist[i+1].x;
			float y2 = polylist[k].pointlist[i+1].y;
			
			float dx = x2 - x1;
			float dy = y2 - y1;
			
			float distance = abs( dy * x - dx * y - x1 * y2 + x2 * y1) / sqrt(dx * dx + dy * dy);
			
			
			//Expand, shift all values past the selected one down by one
			if(distance < 5 && 
			((x1 < x && x2 > x) || (x1 > x && x2 < x)) &&
			((y1 < y && y2 > y) || (y1 > y && y2 < y))){
				
				polylist[k].pointlist.resize(polylist[k].pointlist.size() + 1);
				polylist[k].line_list.resize(polylist[k].line_list.size() + 1);
				
				for(int j =polylist[k].pointlist.size() - 1; j > i + 1; j--){
					polylist[k].pointlist[j] = polylist[k].pointlist[j-1];
					
				}
				polylist[k].pointlist[i+1].x = x;
				polylist[k].pointlist[i+1].y = y;
				
				point_selected = true;
				polygon_selected_index = k;
				point_selected_index = i+1;

				original_selected_position.x = polylist[k].pointlist[i+1].x;
				original_selected_position.y = polylist[k].pointlist[i+1].y;
				
				return;
				
			}
			
		}
		//Edge of polygon
		{
			//Find the distance between the point and the line
			float x1 = polylist[k].pointlist[0].x;
			float y1 = polylist[k].pointlist[0].y;
			
			float x2 = polylist[k].pointlist[polylist[k].pointlist.size() - 1].x;
			float y2 = polylist[k].pointlist[polylist[k].pointlist.size() - 1].y;
			
			float dx = x2 - x1;
			float dy = y2 - y1;
			
			float distance = abs( dy * x - dx * y - x1 * y2 + x2 * y1) / sqrt(dx * dx + dy * dy);
			
			
			//Expand, shift all values past the selected one down by one
			if(distance < 5 && 
			((x1 < x && x2 > x) || (x1 > x && x2 < x)) &&
			((y1 < y && y2 > y) || (y1 > y && y2 < y))){
				
				polylist[k].pointlist.resize(polylist[k].pointlist.size() + 1);
				polylist[k].line_list.resize(polylist[k].line_list.size() + 1);
				
				
				polylist[k].pointlist[polylist[k].pointlist.size() - 1].x = x;
				polylist[k].pointlist[polylist[k].pointlist.size() - 1].y = y;
				
				point_selected = true;
				polygon_selected_index = k;
				point_selected_index = polylist[k].pointlist.size() - 1;

				original_selected_position.x = polylist[k].pointlist[polylist[k].pointlist.size() - 1].x;
				original_selected_position.y = polylist[k].pointlist[polylist[k].pointlist.size() - 1].y;
				
				return;
				
			}

		}

	}
}

//--------------------------------------------------------------
void testApp::mouseReleased(int x, int y, int button){
	if(point_selected){
		if( (x - original_selected_position.x) * (x - original_selected_position.x) + (y - original_selected_position.y) * (y - original_selected_position.y) < 9){
			//Delete the point
			for(int i = point_selected_index; i < polylist[polygon_selected_index].pointlist.size() - 1; i++){

				polylist[polygon_selected_index].pointlist[i] = polylist[polygon_selected_index].pointlist[i+1];

			}
			polylist[polygon_selected_index].pointlist.resize(polylist[polygon_selected_index].pointlist.size() - 1);
			polylist[polygon_selected_index].line_list.resize(polylist[polygon_selected_index].pointlist.size() - 1);

		}

		point_selected = false;
	}
	else if (light_placement){

		current_light_index++;
		
		Light new_light;
		
		/*myColor black;
		black.r = 0;
		black.g = 0;
		black.b = 0;*/
		myColor white;
		white.r = 255;
		white.g = 255;
		white.b = 255;

		new_light.color = white;
		
		light_list.push_back(new_light);

		light_list[light_list.size() - 1].position.x = x;
		light_list[light_list.size() - 1].position.y = y;

		light_placement = false;
	}
	else{
		
		
		myPoint newpoint;
		newpoint.x = x;
		newpoint.y = y;

		polylist[current_poly_index].pointlist.push_back(newpoint);
		myLine newline;
		polylist[current_poly_index].line_list.push_back(newline);
		//pointlist.push_back(newpoint);
	}
}

//--------------------------------------------------------------
void testApp::windowResized(int w, int h){

}

//--------------------------------------------------------------
void testApp::gotMessage(ofMessage msg){

}

//--------------------------------------------------------------
void testApp::dragEvent(ofDragInfo dragInfo){ 

}


void testApp::draw_Polygon(myPolygon polygon){
	//Find a "corner"
	
	int size = polygon.pointlist.size();
	if (size < 3) return;

	if (size == 3){
		ofFill();
		ofSetColor(polygon.color.r, polygon.color.g, polygon.color.b);

		ofTriangle(polygon.pointlist[0].x,polygon.pointlist[0].y,
					polygon.pointlist[1].x,polygon.pointlist[1].y,
					polygon.pointlist[2].x,polygon.pointlist[2].y);
		return;
	}

	for(int i = 0; i < polygon.pointlist.size(); i++){
		
		bool corner = true;

		//Check: is each other point on the opposite side of the line?
		if (i == 0){

			float x_3 = polygon.pointlist[0].x;
			float y_3 = polygon.pointlist[0].y;
					
			float x_1 = polygon.pointlist[1].x;
			float y_1 = polygon.pointlist[1].y;
					
			float x_2 = polygon.pointlist[size - 1].x;
			float y_2 = polygon.pointlist[size - 1].y;

			for(int j = 0; j < size; j++){
				if (j != 0 && j != size - 1 && j != 1){
					
					float a_x = polygon.pointlist[j].x;
					float a_y = polygon.pointlist[j].y;
					
					
					
					
					//Is there another point which is inside the triangle?
					if ( (((y_1 - y_2)*(a_x - x_1) + (x_2 - x_1) * (a_y - y_1)) *
					     ((y_1 - y_2)*(x_3 - x_1) + (x_2 - x_1) * (y_3 - y_1)) > 0 ) &&
						 
						 (((y_1 - y_3)*(a_x - x_1) + (x_3 - x_1) * (a_y - y_1)) *
						  ((y_1 - y_3) * (x_2 - x_1) + (x_3 - x_1) * (y_2 - y_1)) > 0) &&
						 
						 (((y_3 - y_2)*(a_x - x_3) + (x_2 - x_3) * (a_y - y_3)) *
						  ((y_3 - y_2)*(x_1 - x_3) + (x_2 - x_3) * (y_1 - y_3)) > 0))
						 
					{
						corner = false;
						break;
					}
					

				}
			}
			if (!corner) continue;

			//Is the line between inside the polygon?
			int num_intersections = 0;

			float avg_x = (x_1 + x_2)/2;
			float avg_y = (y_1 + y_2)/2;

			for(int k = 0; k < size - 1; k++){

				if( (polygon.pointlist[k].y < avg_y && polygon.pointlist[k+1].y > avg_y) ||
					(polygon.pointlist[k].y > avg_y && polygon.pointlist[k+1].y < avg_y)){

						float y_diff_1 =  polygon.pointlist[k].y - avg_y ;

						float line_x = polygon.pointlist[k].x + (polygon.pointlist[k+1].x - polygon.pointlist[k].x) * y_diff_1 / (polygon.pointlist[k].y - polygon.pointlist[k+1].y);
						if (line_x > avg_x) num_intersections++;

				}

			}

			if( (polygon.pointlist[0].y < avg_y && polygon.pointlist[size - 1].y > avg_y) ||
					(polygon.pointlist[0].y > avg_y && polygon.pointlist[size - 1].y < avg_y)){

						float y_diff_1 =  polygon.pointlist[0].y - avg_y ;
						

						float line_x = polygon.pointlist[0].x + (polygon.pointlist[size-1].x - polygon.pointlist[0].x) * y_diff_1 / (polygon.pointlist[0].y - polygon.pointlist[size - 1].y);
						if (line_x > avg_x)  num_intersections++;

			}

			if(num_intersections % 2 == 0){
				corner = false; 
				continue;
			}

			if (corner){
				ofFill();
				ofSetColor(polygon.color.r, polygon.color.g, polygon.color.b);
				
				ofTriangle(polygon.pointlist[0].x, polygon.pointlist[0].y, 
				polygon.pointlist[size - 1].x, polygon.pointlist[size -1].y, 
				polygon.pointlist[1].x, polygon.pointlist[1].y);
				
				if (size > 3){
					 
					polygon.pointlist.erase(polygon.pointlist.begin());
					draw_Polygon(polygon);
				}
				return;
				
			}

		}

		else if (i == size - 1){

			float x_3 = polygon.pointlist[size - 1].x;
			float y_3 = polygon.pointlist[size - 1].y;
					
			float x_1 = polygon.pointlist[0].x;
			float y_1 = polygon.pointlist[0].y;
					
			float x_2 = polygon.pointlist[size - 2].x;
			float y_2 = polygon.pointlist[size - 2].y;

			for(int j = 0; j < size; j++){
				if (j != 0 && j != size - 1 && j != size - 2){
					
					float a_x = polygon.pointlist[j].x;
					float a_y = polygon.pointlist[j].y;
					
					//Is there another point which is inside the triangle?
					if ( (((y_1 - y_2)*(a_x - x_1) + (x_2 - x_1) * (a_y - y_1)) *
					     ((y_1 - y_2)*(x_3 - x_1) + (x_2 - x_1) * (y_3 - y_1)) > 0 ) &&
						 
						 (((y_1 - y_3)*(a_x - x_1) + (x_3 - x_1) * (a_y - y_1)) *
						  ((y_1 - y_3) * (x_2 - x_1) + (x_3 - x_1) * (y_2 - y_1)) > 0) &&
						 
						 (((y_3 - y_2)*(a_x - x_3) + (x_2 - x_3) * (a_y - y_3)) *
						  ((y_3 - y_2)*(x_1 - x_3) + (x_2 - x_3) * (y_1 - y_3)) > 0))
						 
					{
						corner = false;
						break;
					}

				}
			}

			if (!corner) continue;

			//Is the line between inside the polygon?
			int num_intersections = 0;

			float avg_x = (x_1 + x_2)/2;
			float avg_y = (y_1 + y_2)/2;

			for(int k = 0; k < size - 1; k++){

				if( (polygon.pointlist[k].y < avg_y && polygon.pointlist[k+1].y > avg_y) ||
					(polygon.pointlist[k].y > avg_y && polygon.pointlist[k+1].y < avg_y)){

						float y_diff_1 =  polygon.pointlist[k].y - avg_y ;

						float line_x = polygon.pointlist[k].x + (polygon.pointlist[k+1].x - polygon.pointlist[k].x) * y_diff_1 / (polygon.pointlist[k].y - polygon.pointlist[k+1].y);
						if (line_x > avg_x) num_intersections++;

				}

			}

			if( (polygon.pointlist[0].y < avg_y && polygon.pointlist[size - 1].y > avg_y) ||
					(polygon.pointlist[0].y > avg_y && polygon.pointlist[size - 1].y < avg_y)){

						float y_diff_1 =  polygon.pointlist[0].y - avg_y ;
						

						float line_x = polygon.pointlist[0].x + (polygon.pointlist[size-1].x - polygon.pointlist[0].x) * y_diff_1 / (polygon.pointlist[0].y - polygon.pointlist[size - 1].y);
						if (line_x > avg_x)  num_intersections++;

			}

			if(num_intersections % 2 == 0){
				corner = false; 
				continue;
			}

			if (corner){
				ofFill();
				ofSetColor(polygon.color.r, polygon.color.g, polygon.color.b);
				
				ofTriangle(polygon.pointlist[0].x, polygon.pointlist[0].y, 
				polygon.pointlist[size - 1].x, polygon.pointlist[size -1].y, 
				polygon.pointlist[size - 2].x, polygon.pointlist[size - 2].y);
				
				if (size > 3){
				
					polygon.pointlist.erase(polygon.pointlist.end() - 1);
					draw_Polygon(polygon);
					
				}
				return;
				
			}

		}
		
		else{

			float x_3 = polygon.pointlist[i].x;
			float y_3 = polygon.pointlist[i].y;
					
			float x_1 = polygon.pointlist[i + 1].x;
			float y_1 = polygon.pointlist[i + 1].y;
					
			float x_2 = polygon.pointlist[i - 1].x;
			float y_2 = polygon.pointlist[i - 1].y;

			for(int j = 0; j < size; j++){
				if (j != i && j != i- 1 && j !=  i + 1){
					
					float a_x = polygon.pointlist[j].x;
					float a_y = polygon.pointlist[j].y;
					
					//Is there another point which is inside the triangle?
					if ( (((y_1 - y_2)*(a_x - x_1) + (x_2 - x_1) * (a_y - y_1)) *
					     ((y_1 - y_2)*(x_3 - x_1) + (x_2 - x_1) * (y_3 - y_1)) > 0 ) &&
						 
						 (((y_1 - y_3)*(a_x - x_1) + (x_3 - x_1) * (a_y - y_1)) *
						  ((y_1 - y_3) * (x_2 - x_1) + (x_3 - x_1) * (y_2 - y_1)) > 0) &&
						 
						 (((y_3 - y_2)*(a_x - x_3) + (x_2 - x_3) * (a_y - y_3)) *
						  ((y_3 - y_2)*(x_1 - x_3) + (x_2 - x_3) * (y_1 - y_3)) > 0))
						 
					{
						corner = false;
						break;
					}
					

				}
			}

			if (!corner) continue;

			//Is the line between inside the polygon?
			int num_intersections = 0;

			float avg_x = (x_1 + x_2)/2;
			float avg_y = (y_1 + y_2)/2;

			for(int k = 0; k < size - 1; k++){

				if( (polygon.pointlist[k].y < avg_y && polygon.pointlist[k+1].y > avg_y) ||
					(polygon.pointlist[k].y > avg_y && polygon.pointlist[k+1].y < avg_y)){

						float y_diff_1 =  polygon.pointlist[k].y - avg_y ;

						float line_x = polygon.pointlist[k].x + (polygon.pointlist[k+1].x - polygon.pointlist[k].x) * y_diff_1 / (polygon.pointlist[k].y - polygon.pointlist[k+1].y);
						if (line_x > avg_x) num_intersections++;

				}

			}

			if( (polygon.pointlist[0].y < avg_y && polygon.pointlist[size - 1].y > avg_y) ||
					(polygon.pointlist[0].y > avg_y && polygon.pointlist[size - 1].y < avg_y)){

						float y_diff_1 =  polygon.pointlist[0].y - avg_y ;
						

						float line_x = polygon.pointlist[0].x + (polygon.pointlist[size-1].x - polygon.pointlist[0].x) * y_diff_1 / (polygon.pointlist[0].y - polygon.pointlist[size - 1].y);
						if (line_x > avg_x)  num_intersections++;

			}

			if(num_intersections % 2 == 0){
				corner = false; 
				continue;
			}
			if (corner){

				ofFill();
				ofSetColor(polygon.color.r, polygon.color.g, polygon.color.b);
				
				ofTriangle(polygon.pointlist[i].x, polygon.pointlist[i].y, 
				polygon.pointlist[i - 1].x, polygon.pointlist[i - 1].y, 
				polygon.pointlist[i + 1].x, polygon.pointlist[i + 1].y);
				
				if (size > 3){
					
					polygon.pointlist.erase(polygon.pointlist.begin() + i);
					draw_Polygon(polygon);
					
				}
				return;
				
			}
		
		
		}


	}

}

intersection_information testApp::Closest_Intersection(myPoint origin, myPoint direction, float distance_travelled, int num_reflections, bool mirrors_on,
	int destination_poly_index, int destination_point_index){


	bool hit_target = false;

	myPoint closest_point;

	int closest_polygon_num = -1;

	int closest_edge_num = -1;


	//Normalise direction vector
	float target_distance = sqrt(direction.x * direction.x + direction.y * direction.y);

	direction.x /= target_distance;
	direction.y /= target_distance;

	closest_point.x = origin.x + direction.x * 2000;
	closest_point.y = origin.y + direction.y * 2000;

	float closest_distance = 2000;
	float closest_edge_distance = 0;


	for(int j = 0; j < polylist.size(); j++){

		for(int k = 0; k < polylist[j].pointlist.size(); k++){

			int k_plus_one;
			int k_minus_one;
			if(k == polylist[j].pointlist.size() - 1){
				k_plus_one = 0;
			}
			else{
				k_plus_one = k+1;
			}
			if (k == 0){
				k_minus_one = polylist[j].pointlist.size() - 1;
			}
			else{
				k_minus_one = k - 1;
			}

			if (destination_poly_index == j && destination_point_index == k_plus_one){
				continue;
			}

			//checking at the destination point point
			else if (destination_poly_index == j && destination_point_index == k){
				
				//Are the two lines here on opposite sides of the direction vector?

				myPoint line_1;
				myPoint line_2;

				line_1.x = polylist[j].pointlist[k_plus_one].x - polylist[j].pointlist[k].x;
				line_1.y = polylist[j].pointlist[k_plus_one].y - polylist[j].pointlist[k].y;

				line_2.x =  polylist[j].pointlist[k_minus_one].x - polylist[j].pointlist[k].x;
				line_2.y =  polylist[j].pointlist[k_minus_one].y - polylist[j].pointlist[k].y;

				myPoint rotated_line_1;
				myPoint rotated_line_2;

				rotated_line_1.x = direction.x * line_1.x + direction.y * line_1.y;
				rotated_line_1.y = direction.x * line_1.y - direction.y * line_1.x;

				rotated_line_2.x = direction.x * line_2.x + direction.y * line_2.y;
				rotated_line_2.y = direction.x * line_2.y - direction.y * line_2.x;

				if ( (rotated_line_2.y > 0 && rotated_line_1.y > 0) ||
				     (rotated_line_2.y < 0 && rotated_line_1.y < 0)){

						 //Then the lines are on the same side of the direction vector, so no intersection
						 continue;

				}
				else{

					myPoint origin_to_destination;

					origin_to_destination.x = polylist[j].pointlist[k].x - origin.x;
					origin_to_destination.y = polylist[j].pointlist[k].y - origin.y;
					
					float distance = sqrt(origin_to_destination.x * origin_to_destination.x + origin_to_destination.y * origin_to_destination.y);

					if( distance < closest_distance && distance > 0){
						closest_distance = distance;
						closest_edge_distance = 0;
						closest_polygon_num = j;
						closest_edge_num = k;
						closest_point = polylist[j].pointlist[k];
						hit_target = true;
					}
				}

			}


			else{
				myPoint origin_2;
				origin_2.x = polylist[j].pointlist[k].x;
				origin_2.y = polylist[j].pointlist[k].y;

				myPoint direction_2;
				direction_2.x = polylist[j].pointlist[k_plus_one].x - polylist[j].pointlist[k].x;
				direction_2.y = polylist[j].pointlist[k_plus_one].y - polylist[j].pointlist[k].y;

				myPoint origin_difference;
				origin_difference.x = origin.x - origin_2.x;
				origin_difference.y = origin.y - origin_2.y;

				float direction_2_distance;

				direction_2_distance = ( direction.x * (origin_2.y - origin.y) + direction.y * (origin.x - origin_2.x)) /
										(direction_2.x * direction.y - direction_2.y * direction.x);
				
				

				//Is the intersection point on the line segment?
				if (direction_2_distance < 1 && direction_2_distance > 0){

					float direction_1_distance = ( direction_2.x * (origin.y - origin_2.y) + direction_2.y * (origin_2.x - origin.x)) /
												 ( direction.x * direction_2.y - direction.y * direction_2.x);

					//This distance is the proper distance from the origin to the intersetion point

					if (direction_1_distance < closest_distance && direction_1_distance > 0){

						myPoint intersection_point;
						intersection_point.x = origin.x + direction.x * direction_1_distance;
						intersection_point.y = origin.y + direction.y * direction_1_distance;

						closest_point = intersection_point;
						closest_distance = direction_1_distance;
					
						closest_polygon_num = j;
						closest_edge_num = k;
						closest_edge_distance = direction_2_distance;
						hit_target = false;

					}


				}
			}
			
		}

	}

	if(view_rays){

		ofLine(origin.x, origin.y, closest_point.x, closest_point.y);
	}

	intersection_information return_value;

	return_value.distance = sqrt((closest_point.x - origin.x) * (closest_point.x - origin.x) + (closest_point.y - origin.y) * (closest_point.y - origin.y))
							+ distance_travelled;

	return_value.polygon_num = closest_polygon_num;
	return_value.edge_num = closest_edge_num;
	if (return_value.distance > target_distance && destination_poly_index != -1){
		return_value.passed_target = true;
		return_value.passed_polygon_num = destination_poly_index;
		return_value.passed_point_num = destination_point_index;
	}
	else{
		return_value.passed_target = false;
	}
	return_value.edge_distance = closest_edge_distance;
	return_value.hit_target  = hit_target;

	last_mirror_polygon = -1;
	last_mirror_index_1 = -1;
	last_mirror_index_2 = -1;

	return return_value;
}

bool testApp::polygonsIntersect(myPolygon polygon1, myPolygon polygon2){


	//Use the Separating Axis theorem!
	//(This determines whether two polygons intersect by projecting each of them 
	//onto each possible "separating axis" between them -- in this case, we use the normals of
	//the edges of both polygons

	myPolygon smaller_polygon;
	myPolygon larger_polygon;
	if(polygon1.pointlist.size() >= polygon2.pointlist.size()){
		
		smaller_polygon = polygon2;
		larger_polygon = polygon1;

	}
	else {

		smaller_polygon = polygon1;
		larger_polygon = polygon2;
	}
	//Go through each side of the smaller polygon
	for(int i = 0; i < smaller_polygon.pointlist.size() - 1; i ++){
		
		//These two points determine the axis
		myPoint point1 = smaller_polygon.pointlist[i];
		myPoint point2 = smaller_polygon.pointlist[i+1];

		//This is the vector we project each point onto, after translating the point
		//(it is orthogonal to the side vector)
		myPoint axis_vector;
		axis_vector.y = -(point2.x - point1.x);
		axis_vector.x = (point2.y - point1.y);

		ofSetColor(0,0,255);
		ofLine(point1.x, point1.y, point1.x + axis_vector.x, point1.y + axis_vector.y);

		//First determine the projection of the smaller polygon onto the axis
		float smaller_polygon_projection_bottom;
		float smaller_polygon_projection_top;
		for(int j = 0; j < smaller_polygon.pointlist.size(); j++){
			
			myPoint point_vector;
			point_vector.x = smaller_polygon.pointlist[j].x - point1.x;
			point_vector.y = smaller_polygon.pointlist[j].y - point1.y;

			float point_dot_axis = point_vector.x * axis_vector.x + point_vector.y * axis_vector.y;
			float axis_dot_axis = axis_vector.x * axis_vector.x + axis_vector.y * axis_vector.y;

			float point_projection = point_dot_axis / axis_dot_axis;

			if(j == 0){
				smaller_polygon_projection_bottom = point_projection;
				smaller_polygon_projection_top = point_projection;
			}
			else{

				if (point_projection < smaller_polygon_projection_bottom){
					smaller_polygon_projection_bottom = point_projection;
				}
				if(point_projection > smaller_polygon_projection_top){
					smaller_polygon_projection_top = point_projection;
				}
			}

		}
		ofSetColor(0, 0, 255);
		ofCircle(point1.x + axis_vector.x * smaller_polygon_projection_top,point1.y + axis_vector.y * smaller_polygon_projection_top, 5);
		ofCircle(point1.x + axis_vector.x * smaller_polygon_projection_bottom,point1.y + axis_vector.y * smaller_polygon_projection_bottom, 5);

		float larger_polygon_projection_bottom;
		float larger_polygon_projection_top;
		for(int j = 0; j < larger_polygon.pointlist.size(); j++){
			
			myPoint point_vector;
			point_vector.x = larger_polygon.pointlist[j].x - point1.x;
			point_vector.y = larger_polygon.pointlist[j].y - point1.y;

			float point_dot_axis = point_vector.x * axis_vector.x + point_vector.y * axis_vector.y;
			float axis_dot_axis = axis_vector.x * axis_vector.x + axis_vector.y * axis_vector.y;

			float point_projection = point_dot_axis / axis_dot_axis;

			if(j == 0){
				larger_polygon_projection_bottom = point_projection;
				larger_polygon_projection_top = point_projection;
			}
			else{

				if (point_projection < larger_polygon_projection_bottom){
					larger_polygon_projection_bottom = point_projection;
				}
				if(point_projection > larger_polygon_projection_top){
					larger_polygon_projection_top = point_projection;
				}
			}

		}
		ofSetColor(255, 0,0);
		ofCircle(point1.x + axis_vector.x * larger_polygon_projection_top,point1.y + axis_vector.y * larger_polygon_projection_top, 5);
		ofCircle(point1.x + axis_vector.x * larger_polygon_projection_bottom,point1.y + axis_vector.y * larger_polygon_projection_bottom, 5);
		//Do the two projections overlap? If not, the polygons do not overlap

		if((smaller_polygon_projection_bottom > larger_polygon_projection_bottom && smaller_polygon_projection_bottom < larger_polygon_projection_top)||
		   (smaller_polygon_projection_top > larger_polygon_projection_bottom && smaller_polygon_projection_top < larger_polygon_projection_top)||
		   (larger_polygon_projection_bottom > smaller_polygon_projection_bottom && larger_polygon_projection_bottom < smaller_polygon_projection_top)||
		   (larger_polygon_projection_top > smaller_polygon_projection_bottom && larger_polygon_projection_top < smaller_polygon_projection_top))
		{

			   //The projections overlap

		}
		else{
			return false;
		}

	}
	//Don't forget the final edge of the smaller polygon
	{
		//These two points determine the axis
		myPoint point1 = smaller_polygon.pointlist[0];
		myPoint point2 = smaller_polygon.pointlist[smaller_polygon.pointlist.size() - 1];

		//This is the vector we project each point onto, after translating the point
		myPoint axis_vector;
		axis_vector.y = -(point2.x - point1.x);
		axis_vector.x = (point2.y - point1.y);
		ofLine(point1.x, point1.y, point1.x + axis_vector.x, point1.y + axis_vector.y);

		//First determine the projection of the smaller polygon onto the axis
		float smaller_polygon_projection_bottom;
		float smaller_polygon_projection_top;
		for(int j = 0; j < smaller_polygon.pointlist.size(); j++){
			
			myPoint point_vector;
			point_vector.x = smaller_polygon.pointlist[j].x - point1.x;
			point_vector.y = smaller_polygon.pointlist[j].y - point1.y;

			float point_dot_axis = point_vector.x * axis_vector.x + point_vector.y * axis_vector.y;
			float axis_dot_axis = axis_vector.x * axis_vector.x + axis_vector.y * axis_vector.y;

			float point_projection = point_dot_axis / axis_dot_axis;

			if(j == 0){
				smaller_polygon_projection_bottom = point_projection;
				smaller_polygon_projection_top = point_projection;
			}
			else{

				if (point_projection < smaller_polygon_projection_bottom){
					smaller_polygon_projection_bottom = point_projection;
				}
				if(point_projection > smaller_polygon_projection_top){
					smaller_polygon_projection_top = point_projection;
				}
			}

		}
		ofSetColor(0, 0, 255);
		ofCircle(point1.x + axis_vector.x * smaller_polygon_projection_top,point1.y + axis_vector.y * smaller_polygon_projection_top, 5);
		ofCircle(point1.x + axis_vector.x * smaller_polygon_projection_bottom,point1.y + axis_vector.y * smaller_polygon_projection_bottom, 5);

		float larger_polygon_projection_bottom;
		float larger_polygon_projection_top;
		for(int j = 0; j < larger_polygon.pointlist.size(); j++){
			
			myPoint point_vector;
			point_vector.x = larger_polygon.pointlist[j].x - point1.x;
			point_vector.y = larger_polygon.pointlist[j].y - point1.y;

			float point_dot_axis = point_vector.x * axis_vector.x + point_vector.y * axis_vector.y;
			float axis_dot_axis = axis_vector.x * axis_vector.x + axis_vector.y * axis_vector.y;

			float point_projection = point_dot_axis / axis_dot_axis;

			if(j == 0){
				larger_polygon_projection_bottom = point_projection;
				larger_polygon_projection_top = point_projection;
			}
			else{

				if (point_projection < larger_polygon_projection_bottom){
					larger_polygon_projection_bottom = point_projection;
				}
				if(point_projection > larger_polygon_projection_top){
					larger_polygon_projection_top = point_projection;
				}
			}

		}
		ofSetColor(255, 0,0);
		ofCircle(point1.x + axis_vector.x * larger_polygon_projection_top,point1.y + axis_vector.y * larger_polygon_projection_top, 5);
		ofCircle(point1.x + axis_vector.x * larger_polygon_projection_bottom,point1.y + axis_vector.y * larger_polygon_projection_bottom, 5);
		//Do the two projections overlap? If not, the polygons do not overlap

		if((smaller_polygon_projection_bottom > larger_polygon_projection_bottom && smaller_polygon_projection_bottom < larger_polygon_projection_top)||
		   (smaller_polygon_projection_top > larger_polygon_projection_bottom && smaller_polygon_projection_top < larger_polygon_projection_top)||
		   (larger_polygon_projection_bottom > smaller_polygon_projection_bottom && larger_polygon_projection_bottom < smaller_polygon_projection_top)||
		   (larger_polygon_projection_top > smaller_polygon_projection_bottom && larger_polygon_projection_top < smaller_polygon_projection_top))
		{

			   //The projections overlap

		}
		else{
			return false;
		}


	}


	//Now go through each side of the larger polygon
	for(int i = 0; i < larger_polygon.pointlist.size() - 1; i ++){
		
		//These two points determine the axis
		myPoint point1 = larger_polygon.pointlist[i];
		myPoint point2 = larger_polygon.pointlist[i+1];

		//This is the vector we project each point onto, after translating the point
		myPoint axis_vector;
		axis_vector.y = -(point2.x - point1.x);
		axis_vector.x = (point2.y - point1.y);


		//First determine the projection of the smaller polygon onto the axis
		float smaller_polygon_projection_bottom;
		float smaller_polygon_projection_top;
		for(int j = 0; j < smaller_polygon.pointlist.size(); j++){
			
			myPoint point_vector;
			point_vector.x = smaller_polygon.pointlist[j].x - point1.x;
			point_vector.y = smaller_polygon.pointlist[j].y - point1.y;

			float point_dot_axis = point_vector.x * axis_vector.x + point_vector.y * axis_vector.y;
			float axis_dot_axis = axis_vector.x * axis_vector.x + axis_vector.y * axis_vector.y;

			float point_projection = point_dot_axis / axis_dot_axis;

			if(j == 0){
				smaller_polygon_projection_bottom = point_projection;
				smaller_polygon_projection_top = point_projection;
			}
			else{

				if (point_projection < smaller_polygon_projection_bottom){
					smaller_polygon_projection_bottom = point_projection;
				}
				if(point_projection > smaller_polygon_projection_top){
					smaller_polygon_projection_top = point_projection;
				}
			}

		}

		float larger_polygon_projection_bottom;
		float larger_polygon_projection_top;
		for(int j = 0; j < larger_polygon.pointlist.size(); j++){
			
			myPoint point_vector;
			point_vector.x = larger_polygon.pointlist[j].x - point1.x;
			point_vector.y = larger_polygon.pointlist[j].y - point1.y;

			float point_dot_axis = point_vector.x * axis_vector.x + point_vector.y * axis_vector.y;
			float axis_dot_axis = axis_vector.x * axis_vector.x + axis_vector.y * axis_vector.y;

			float point_projection = point_dot_axis / axis_dot_axis;

			if(j == 0){
				larger_polygon_projection_bottom = point_projection;
				larger_polygon_projection_top = point_projection;
			}
			else{

				if (point_projection < larger_polygon_projection_bottom){
					larger_polygon_projection_bottom = point_projection;
				}
				if(point_projection > larger_polygon_projection_top){
					larger_polygon_projection_top = point_projection;
				}
			}

		}

		//Do the two projections overlap? If not, the polygons do not overlap

		if((smaller_polygon_projection_bottom > larger_polygon_projection_bottom && smaller_polygon_projection_bottom < larger_polygon_projection_top)||
		   (smaller_polygon_projection_top > larger_polygon_projection_bottom && smaller_polygon_projection_top < larger_polygon_projection_top)||
		   (larger_polygon_projection_bottom > smaller_polygon_projection_bottom && larger_polygon_projection_bottom < smaller_polygon_projection_top)||
		   (larger_polygon_projection_top > smaller_polygon_projection_bottom && larger_polygon_projection_top < smaller_polygon_projection_top))
		{

			   //The projections overlap

		}
		else{
			return false;
		}

	}
	//Don't forget the final edge of the larger polygon
	{
		//These two points determine the axis
		myPoint point1 = larger_polygon.pointlist[0];
		myPoint point2 = larger_polygon.pointlist[larger_polygon.pointlist.size() - 1];

		//This is the vector we project each point onto, after translating the point
		myPoint axis_vector;
		axis_vector.y = -(point2.x - point1.x);
		axis_vector.x = (point2.y - point1.y);


		//First determine the projection of the smaller polygon onto the axis
		float smaller_polygon_projection_bottom;
		float smaller_polygon_projection_top;
		for(int j = 0; j < smaller_polygon.pointlist.size(); j++){
			
			myPoint point_vector;
			point_vector.x = smaller_polygon.pointlist[j].x - point1.x;
			point_vector.y = smaller_polygon.pointlist[j].y - point1.y;

			float point_dot_axis = point_vector.x * axis_vector.x + point_vector.y * axis_vector.y;
			float axis_dot_axis = axis_vector.x * axis_vector.x + axis_vector.y * axis_vector.y;

			float point_projection = point_dot_axis / axis_dot_axis;

			if(j == 0){
				smaller_polygon_projection_bottom = point_projection;
				smaller_polygon_projection_top = point_projection;
			}
			else{

				if (point_projection < smaller_polygon_projection_bottom){
					smaller_polygon_projection_bottom = point_projection;
				}
				if(point_projection > smaller_polygon_projection_top){
					smaller_polygon_projection_top = point_projection;
				}
			}

		}

		float larger_polygon_projection_bottom;
		float larger_polygon_projection_top;
		for(int j = 0; j < larger_polygon.pointlist.size(); j++){
			
			myPoint point_vector;
			point_vector.x = larger_polygon.pointlist[j].x - point1.x;
			point_vector.y = larger_polygon.pointlist[j].y - point1.y;

			float point_dot_axis = point_vector.x * axis_vector.x + point_vector.y * axis_vector.y;
			float axis_dot_axis = axis_vector.x * axis_vector.x + axis_vector.y * axis_vector.y;

			float point_projection = point_dot_axis / axis_dot_axis;

			if(j == 0){
				larger_polygon_projection_bottom = point_projection;
				larger_polygon_projection_top = point_projection;
			}
			else{

				if (point_projection < larger_polygon_projection_bottom){
					larger_polygon_projection_bottom = point_projection;
				}
				if(point_projection > larger_polygon_projection_top){
					larger_polygon_projection_top = point_projection;
				}
			}

		}

		//Do the two projections overlap? If not, the polygons do not overlap

		if((smaller_polygon_projection_bottom > larger_polygon_projection_bottom && smaller_polygon_projection_bottom < larger_polygon_projection_top)||
		   (smaller_polygon_projection_top > larger_polygon_projection_bottom && smaller_polygon_projection_top < larger_polygon_projection_top)||
		   (larger_polygon_projection_bottom > smaller_polygon_projection_bottom && larger_polygon_projection_bottom < smaller_polygon_projection_top)||
		   (larger_polygon_projection_top > smaller_polygon_projection_bottom && larger_polygon_projection_top < smaller_polygon_projection_top))
		{

			   //The projections overlap

		}
		else{
			return false;
		}


	}

	//If they overlap on all possible separating axes, they overlap in real life
	return true;

}

void testApp::Color_Line_Segment(int polygon_iter, int side_iter, float start, float end, myColor color){

	float beginning_point;
	float ending_point;

	if (end > start){
		beginning_point = start;
		ending_point = end;
	}
	else{
		beginning_point = end;
		ending_point = start;

	}
	//The dividers should be sorted at this point
	for(int k = 0; k < polylist[polygon_iter].line_list[side_iter].dividers.size(); k++){

		if ( ( k == polylist[polygon_iter].line_list[side_iter].dividers.size() - 1) ||
			(polylist[polygon_iter].line_list[side_iter].dividers[k] <= beginning_point && 
			polylist[polygon_iter].line_list[side_iter].dividers[k + 1] > beginning_point)){
			
			polylist[polygon_iter].line_list[side_iter].dividers.resize(polylist[polygon_iter].line_list[side_iter].dividers.size() + 1);
			polylist[polygon_iter].line_list[side_iter].colors.resize(polylist[polygon_iter].line_list[side_iter].colors.size() + 1);

			for(int x = polylist[polygon_iter].line_list[side_iter].dividers.size() - 1; x > k + 1; x--){

				polylist[polygon_iter].line_list[side_iter].dividers[x] = polylist[polygon_iter].line_list[side_iter].dividers[x-1];
				polylist[polygon_iter].line_list[side_iter].colors[x] = polylist[polygon_iter].line_list[side_iter].colors[x-1];

			}
			polylist[polygon_iter].line_list[side_iter].dividers[k + 1] = beginning_point;
			polylist[polygon_iter].line_list[side_iter].colors[k + 1] = 
				polylist[polygon_iter].line_list[side_iter].colors[k];
			

			//Now we go through and blend all of the colours until we reach the ending point

			for(int j = k + 1; j < polylist[polygon_iter].line_list[side_iter].dividers.size() ; j++){

				bool end_reached = false;

				//Check: have we reached the end of the interval we wish to colour?
				if ( j == polylist[polygon_iter].line_list[side_iter].dividers.size() - 1 || 
					polylist[polygon_iter].line_list[side_iter].dividers[j + 1] > ending_point){

					polylist[polygon_iter].line_list[side_iter].dividers.resize(polylist[polygon_iter].line_list[side_iter].dividers.size() + 1);
					polylist[polygon_iter].line_list[side_iter].colors.resize(polylist[polygon_iter].line_list[side_iter].colors.size() + 1);

					for(int x = polylist[polygon_iter].line_list[side_iter].dividers.size() - 1; x > j + 1; x--){

						polylist[polygon_iter].line_list[side_iter].dividers[x] = polylist[polygon_iter].line_list[side_iter].dividers[x-1];
						polylist[polygon_iter].line_list[side_iter].colors[x] = polylist[polygon_iter].line_list[side_iter].colors[x-1];

					}

					polylist[polygon_iter].line_list[side_iter].dividers[j + 1] = ending_point;
					polylist[polygon_iter].line_list[side_iter].colors[j + 1] = polylist[polygon_iter].line_list[side_iter].colors[j];

					end_reached = true;

				}
				//Now, blend the colors of each interval with those of our light
				myColor blend_color;
				blend_color.r = (polylist[polygon_iter].line_list[side_iter].colors[j].r + color.r)/2;
				blend_color.g = (polylist[polygon_iter].line_list[side_iter].colors[j].g + color.g)/2;
				blend_color.b = (polylist[polygon_iter].line_list[side_iter].colors[j].b + color.b)/2;

				polylist[polygon_iter].line_list[side_iter].colors[j] = blend_color;

				if (end_reached) break;


			}

			break;
		}


	}

}


void testApp::Update_Polygon_Orientations(){

	//Bounding box orientation reversed
	polylist[0].orientation = COUNTERCLOCKWISE;
	

	for(int j = 1; j < polylist.size(); j++){
		if (polylist[j].pointlist.size() < 2) continue;

		float total_sum = 0;

		for(int k = 0; k < polylist[j].pointlist.size(); k++){

			int k_plus_one;
			if(k == polylist[j].pointlist.size() - 1){
				k_plus_one = 0;
			}
			else{
				k_plus_one = k + 1;
			}

			float x1 = polylist[j].pointlist[k].x;
			float x2 = polylist[j].pointlist[k_plus_one].x;

			float y1 = polylist[j].pointlist[k].y;
			float y2 = polylist[j].pointlist[k_plus_one].y;

			total_sum += (x2 - x1) * (y2 + y1);

		}

		if ( total_sum > 0){
			polylist[j].orientation = COUNTERCLOCKWISE;
		}
		else{
			polylist[j].orientation = CLOCKWISE;
		}

	}

}

void testApp::Add_View_Interval(int polygon_iter, int side_iter, float block_start, float block_end, float projection_start, float projection_end){

	myLine interval_line = polylist[polygon_iter].line_list[side_iter];

	int upwards = 0;
	int downwards = 1;
	int direction;

	if(block_start < block_end){
		direction = upwards;
	}
	else{
		direction = downwards;
	}

	float block_size = abs(block_end - block_start);
	float projection_size = abs(projection_end - projection_start);

	player_viewpoint.dividers.push_back(projection_start);
	player_viewpoint.colors.push_back(interval_line.Color_at_Point(block_start));

	if(direction == upwards){

		int divider_iter = interval_line.Divider_at_Point(block_start);
		while(true){
			divider_iter++; 
			if (divider_iter == interval_line.dividers.size()){
				break;
			}
			if (interval_line.dividers[divider_iter] > block_end){
				break;
			}

			float divider_value = projection_start + (interval_line.dividers[divider_iter] - block_start) / block_size * projection_size; 

			player_viewpoint.dividers.push_back(divider_value);
			player_viewpoint.colors.push_back(interval_line.colors[divider_iter]);

		}

	}
	else if(direction == downwards){

		int divider_iter = interval_line.Divider_at_Point(block_start); 

		while(true){

			if (divider_iter == 0) break;

			if (interval_line.dividers[divider_iter] < block_end){
				break;
			}

			float divider_value = projection_start + ( block_start - interval_line.dividers[divider_iter] ) / block_size * projection_size; 

			player_viewpoint.dividers.push_back(divider_value);
			player_viewpoint.colors.push_back(interval_line.colors[divider_iter - 1]);

			divider_iter -= 1;
		}

	}

}