#include "bonhomme.hpp"


using namespace vcl;



 void bonhomme::setup_dude(std::map<std::string,GLuint>& shaders)
{
    const float radius_body = 0.1f/2;
    const float radius_head = 0.30f/2;
    const float height_body = 1.5f/2;
    const float width_pelvis = 0.25f/2;
    const float width_chest = 4*radius_body;

    //Body
    mesh_drawable body = mesh_drawable( mesh_primitive_cylinder(radius_body, {0,0,0}, {0,0,height_body}));
    //Head
    mesh_drawable head = mesh_drawable( mesh_primitive_sphere(radius_head));
    //Eyes
    mesh_drawable eye = mesh_drawable(mesh_primitive_sphere(0.09f/2, {0,0,0}, 20, 20));
    eye.uniform.color = {0,0,0};
    //Chest
     mesh_drawable chest = mesh_drawable( mesh_primitive_cylinder(radius_body, {0,0,0}, {0,width_chest,0}));
    //Articulation
    mesh_drawable art = mesh_drawable(mesh_primitive_sphere(radius_body));
    //Arms
    mesh_drawable arm = mesh_drawable(mesh_primitive_cylinder(radius_body*0.7f, {0,0,0}, {0,0.1f,-height_body/3}));
    //Pelvis
    mesh_drawable pelvis = mesh_drawable( mesh_primitive_cylinder(radius_body, {0,0,0}, {0,width_pelvis,0}));
    //Legs
    mesh_drawable leg = mesh_drawable(mesh_primitive_cylinder(radius_body*0.85f, {0,0,0}, {0,0.05f,-height_body*4/9}));

    // Build the hierarchy
    hierarchy.add(body, "body");
    hierarchy.add(head, "head", "body", {0,0,height_body + radius_head - 0.1f});
    hierarchy.add(eye, "eye_left", "head" , {radius_head*0.7f,radius_head*1/3,radius_head*2/5});
    hierarchy.add(eye, "eye_right", "head", {radius_head*0.7f,-radius_head*1/3, radius_head*2/5});
    hierarchy.add(chest, "chest", "body", {0,-width_chest/2,4*height_body/5*0.95f});
    hierarchy.add(art, "shoulder_left", "chest", {0, width_chest, 0});
    hierarchy.add(art, "shoulder_right", "chest");
    hierarchy.add(arm, "arm_left", "shoulder_left");
    hierarchy.add(arm, "arm_right", "shoulder_right", {{0,0,0}, {1,0,0, 0,-1,0, 0,0,1}});
    hierarchy.add(art, "elbow_left", "arm_left", {0,0.1f, -height_body/3});
    hierarchy.add(art, "elbow_right", "arm_right", {0,0.1f, -height_body/3});

    arm.uniform.transform.scaling_axis = {1,1,0.9f};

    hierarchy.add(arm, "forearm_left", "elbow_left", {{0,0,0}, {1,0,0, 0,-1,0, 0,0,1}});
    hierarchy.add(arm, "forearm_right","elbow_right", {{0,0,0}, {1,0,0, 0,-1,0, 0,0,1}});
    hierarchy.add(pelvis, "pelvis", "body", {0,-width_pelvis/2,0});
    hierarchy.add(art, "hip_left", "pelvis", {0, width_pelvis,0});
    hierarchy.add(art, "hip_right", "pelvis");
    hierarchy.add(leg, "thigh_left", "hip_left");
    hierarchy.add(leg, "thigh_right","hip_right", {{0,0,0}, {1,0,0, 0,-1,0, 0,0,1}});
    hierarchy.add(art, "knee_left", "thigh_left", {0,0.05f,-height_body*4/9});
    hierarchy.add(art, "knee_right","thigh_right", {0,0.05f,-height_body*4/9});
    hierarchy.add(leg, "leg_left", "knee_left");
    hierarchy.add(leg, "leg_right","knee_right");

    art.uniform.transform.scaling = 0.7f;

    hierarchy.add(art, "hand_left", "forearm_left", {0,0.1f, -height_body/3*0.9f});
    hierarchy.add(art, "hand_right", "forearm_right", {0,0.1f, -height_body/3*0.9f});

    art.uniform.transform.scaling = 0.9f;

    hierarchy.add(art, "foot_left", "leg_left", {0,0.05f, -height_body*4/9});
    hierarchy.add(art, "foot_right", "leg_right", {0,0.05f, -height_body*4/9});

    // Set the same shader for all the elements
    hierarchy.set_shader_for_all_elements(shaders["mesh"]);


    // Initialize helper structure to display the hierarchy skeleton
    hierarchy_visual_debug.init(shaders["segment_im"], shaders["mesh"]);
}




void bonhomme::draw_dude(std::map<std::string,GLuint>& shaders, scene_structure& scene, timer_interval timer, buffer<vec3>& points)
{
    //timer.update();

    // Current time
    const float t = timer.t;
    float dt = 0.1f;


    /** *************************************************************  **/
    /** Compute the (animated) transformations applied to the elements **/
    /** *************************************************************  **/

    const float radius_body = 0.1f/2;
    const float radius_head = 0.30f/2;
    const float height_body = 1.5f/2;
    //const float width_pelvis = 0.25f/2;
    const float width_chest = 4*radius_body;

    float h = hierarchy["body"].transform.translation[2];

    if(!gui_bon.running && h >= 0.0f)
    {
        mat3 const I = {1,0,0, 0,1,0, 0,0,1};
        mat3 const R_y_sym = {1,0,0, 0,-1,0, 0,0,1};
        //Rotation of the head
        mat3 const R_head = rotation_from_axis_angle_mat3({0,1,0}, 0.3f+0.1f*std::sin(2*3.14f*(t)));
        //Movement of the arms
        mat3 const R_fore_rest = rotation_from_axis_angle_mat3({1,0,0}, -0.2f - 0.05f*std::sin(2*3.14f*(t)));

        // Set the rotation to the elements in the hierarchy
        //Oscillation of the shoulders
        hierarchy["chest"].transform.translation = {0,-width_chest/2, 4*height_body/5*0.98f + 0.01f*(1+std::sin(2*3.14f*t))};
        rotation = rotation + 0.1f*gui_bon.turn;
        hierarchy["body"].transform.rotation = rotation_from_axis_angle_mat3({0,0,1}, rotation);
        hierarchy["arm_left"].transform.rotation = I;
        hierarchy["arm_right"].transform.rotation = R_y_sym;
        hierarchy["pelvis"].transform.rotation = I;
        hierarchy["thigh_left"].transform.rotation = I;
        hierarchy["thigh_right"].transform.rotation = R_y_sym;
        hierarchy["leg_left"].transform.rotation = I;
        hierarchy["leg_right"].transform.rotation = I;


        hierarchy["head"].transform.rotation = R_head;
        hierarchy["forearm_left"].transform.rotation = R_fore_rest;
        hierarchy["forearm_right"].transform.rotation = R_fore_rest;
    }
    else if (gui_bon.running && h >= 0.0f)
    {


        //Rotation of the head
        mat3 const R_head = rotation_from_axis_angle_mat3({0,1,0}, 0.2f+0.2f*std::sin(4*3.14f*(t)));
        //Movement of the arms
        mat3 const R_arm_run = rotation_from_axis_angle_mat3({0,1,0}, 0.8f*std::sin(4*3.14f*(t)));
        mat3 const R_forearm_right_run = rotation_from_axis_angle_mat3({0,1,0}, 0.9f + 0.8f*std::sin(4*3.14f*(t)));
        mat3 const R_forearm_left_run = rotation_from_axis_angle_mat3({0,1,0}, -0.9f + 0.8f*std::sin(4*3.14f*(t)));
        mat3 const R_x_sym = {-1,0,0, 0,1,0, 0,0,1};
        mat3 const R_y_sym = {1,0,0, 0,-1,0, 0,0,1};
        //Lean of the body
        mat3 const Lean_body = rotation_from_axis_angle_mat3({0,1,0}, 0.1f - 0.05f*std::cos(8*3.14f*(t)));
        mat3 const Lean_body_inv = rotation_from_axis_angle_mat3({0,1,0}, -0.1f);
        //Legs
        mat3 const R_leg_right_run = rotation_from_axis_angle_mat3({0,1,0}, 0.7f + 0.6f*std::sin(4*3.14f*(t)));
        mat3 const R_leg_left_run = rotation_from_axis_angle_mat3({0,1,0}, -0.7f + 0.6f*std::sin(4*3.14f*(t)));

        // Set the rotation to the elements in the hierarchy
        //hierarchy["body"].transform.rotation = Lean_body*rotation_from_axis_angle_mat3({0,0,1}, gui_bon.turn*2*3.14f*(t));
        rotation = rotation + 0.1f*gui_bon.turn;//rotation_from_axis_angle_mat3({0,0,1}, 0.1f + gui_bon.turn*std::sin(0.5f*3.14f*t));
        hierarchy["body"].transform.rotation = Lean_body*rotation_from_axis_angle_mat3({0,0,1}, rotation);
        hierarchy["head"].transform.rotation = R_head;
        hierarchy["arm_left"].transform.rotation = R_arm_run;
        hierarchy["arm_right"].transform.rotation = R_y_sym*R_x_sym*R_arm_run;
        hierarchy["forearm_left"].transform.rotation = R_forearm_left_run;
        hierarchy["forearm_right"].transform.rotation = R_forearm_right_run;
        hierarchy["pelvis"].transform.rotation = Lean_body_inv;
        hierarchy["thigh_left"].transform.rotation = R_arm_run;
        hierarchy["thigh_right"].transform.rotation = R_y_sym*R_x_sym*R_arm_run;
        hierarchy["leg_left"].transform.rotation = R_leg_right_run;
        hierarchy["leg_right"].transform.rotation = R_leg_left_run;
        hierarchy["chest"].transform.translation = {0,-width_chest/2, 4*height_body/5*0.98f - 0.02f + 0.015f*(1+std::sin(4*3.14f*t))};



        position = position + vec2(dt*std::cos(rotation), dt*std::sin(rotation));
        int N = std::sqrt(points.size());
        int ku = std::min(99.0f, std::floor(N*(position[0]/40 +0.5f)));
        int kv = std::min(99.0f, std::floor(N*(position[1]/40 +0.5f)));
        vec3 v = {position[0], position[1], points[kv+N*ku][2] + height_body*4/5};
        hierarchy["body"].transform.translation = {position[0], position[1], points[kv+N*ku][2] + height_body*4/5};
    }
    else if (gui_bon.running && h < 0.0f)
    {
        mat3 const R_y_sym = {1,0,0, 0,-1,0, 0,0,1};
        mat3 const R_x_sym = {-1,0,0, 0,1,0, 0,0,1};
        mat3 const Lean_body = rotation_from_axis_angle_mat3({0,1,0}, 1.4f - 0.15f*std::cos(3.14f*(t)));
        mat3 const R_head = rotation_from_axis_angle_mat3({0,1,0}, -0.8f+0.2f*std::sin(2*3.14f*(t)));
        mat3 const R_arm = rotation_from_axis_angle_mat3({0,1,0}, 2*3.14f*(t));
        mat3 const R_forearm = rotation_from_axis_angle_mat3({0,1,0}, 0.8f*std::sin(2*3.14f*(t)));
        mat3 const R_leg = rotation_from_axis_angle_mat3({0,1,0}, 0.3f*std::sin(4*3.14f*(t)));

        rotation = rotation + 0.1f*gui_bon.turn;
        hierarchy["body"].transform.rotation = rotation_from_axis_angle_mat3({0,0,1}, rotation)*Lean_body;
        hierarchy["head"].transform.rotation = R_head;
        hierarchy["head"].transform.translation = {-0.1f,0,height_body+radius_head-0.1f};
        hierarchy["arm_left"].transform.rotation = R_arm*rotation_from_axis_angle_mat3({0,1,0}, 3.14f);
        hierarchy["arm_right"].transform.rotation = R_y_sym*R_arm;
        hierarchy["forearm_left"].transform.rotation = R_y_sym*R_forearm;
        hierarchy["forearm_right"].transform.rotation = R_y_sym*R_forearm;
        hierarchy["thigh_left"].transform.rotation = R_leg;
        hierarchy["thigh_right"].transform.rotation = R_y_sym*R_x_sym*R_leg;
        hierarchy["leg_left"].transform.rotation = R_leg;
        hierarchy["leg_right"].transform.rotation = R_leg;

        position = position + vec2(dt*std::cos(rotation), dt*std::sin(rotation));
        hierarchy["body"].transform.translation = {position[0], position[1], -0.05f};

    }
    else if (!gui_bon.running && h < 0.0f)
    {
        mat3 const R_y_sym = {1,0,0, 0,-1,0, 0,0,1};
        mat3 const R_x_sym = {-1,0,0, 0,1,0, 0,0,1};
        mat3 const Lean_body = rotation_from_axis_angle_mat3({0,1,0}, 0.1f*std::cos(3.14f*(t)/2));
        mat3 const R_head = rotation_from_axis_angle_mat3({0,1,0}, 0.2f+0.2f*std::sin(3.14f*(t)/2));
        mat3 const R_arm = rotation_from_axis_angle_mat3({1,0,0}, 0.3f+0.1f*std::sin(3.14f*(t)));
        mat3 const R_forearm = rotation_from_axis_angle_mat3({0,1,0}, 0.8f*std::sin(2*3.14f*(t)));
        mat3 const R_leg = rotation_from_axis_angle_mat3({0,1,0}, 0.5f*std::sin(2*3.14f*(t)));

        rotation = rotation + 0.1f*gui_bon.turn;
        hierarchy["body"].transform.rotation = rotation_from_axis_angle_mat3({0,0,1}, rotation)*Lean_body;
        hierarchy["head"].transform.rotation = R_head;
        hierarchy["chest"].transform.translation = {0,-width_chest/2, 4*height_body/5*0.98f + 0.01f*(1+std::sin(2*3.14f*t))};
        hierarchy["arm_left"].transform.rotation = R_arm;
        hierarchy["arm_right"].transform.rotation = R_y_sym*R_arm;

        hierarchy["thigh_left"].transform.rotation = R_arm;
        hierarchy["thigh_right"].transform.rotation = R_y_sym*R_x_sym*R_arm;
        //hierarchy["leg_left"].transform.rotation = R_leg;
        //hierarchy["leg_right"].transform.rotation = R_leg;
    }



    if (gui_bon.third_person)
    {
        scene.camera.orientation = rotation_from_axis_angle_mat3({0,0,1}, rotation)*rotation_from_axis_angle_mat3({0,1,0}, -1.27f)*rotation_from_axis_angle_mat3({0,0,1}, -1.57f);
        scene.camera.translation = -hierarchy["body"].transform.translation + vec3(0,0,-height_body-0.5f);
        scene.camera.scale = 5.0f;
    }

    hierarchy.update_local_to_global_coordinates();


    /** ********************* **/
    /** Display the hierarchy **/
    /** ********************* **/

    if(gui_bon.surface) // The default display
        draw(hierarchy, scene.camera);

    if(gui_bon.wireframe) // Display the hierarchy as wireframe
        draw(hierarchy, scene.camera, shaders["wireframe"]);

    if(gui_bon.skeleton) // Display the skeleton of the hierarchy (debug)
        hierarchy_visual_debug.draw(hierarchy, scene.camera);

}


void gui_bonhomme_structure::set_gui_bonhomme(bool surf, bool wire)
{
    //ImGui::Checkbox("Display Jack's skeleton", &skeleton);   ImGui::SameLine();
    surface = surf;
    wireframe = wire;
}
