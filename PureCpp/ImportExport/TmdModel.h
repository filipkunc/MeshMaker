//
//  TmdModel.h
//  Original source - RT3D Engine 2.0
//
//  Created by Filip Kunc on 10/18/09.
//  For license see LICENSE.TXT
//

// This format is from my old game.
// It's not so good, but I needed some viewer on Mac for my Windows game.
// So I converted some pieces of code to be platform agnostic.

#pragma once

#include <stdio.h>
#include "../MathCore/MathDeclaration.h"

#define MODEL_VERSION "TMD v.3.1"

struct model_header
{
    char version[10];
	int no_meshes;
	int no_materials;
};

struct anim_header
{
	int no_anims;
	float frame_speed;
	float ticks_per_frame;
	float end_time;
};

struct face
{
    unsigned int vertID[3];
	unsigned int texID[3];
	Vector3D face_normal;
};

struct material
{
	float r, g, b, a;
	char texture_file[255];	
};

struct mesh_desc
{
    int materialID;	
	int no_faces;
    int no_vertices;
	int no_texcoords;
	int start_animID;
	int end_animID;
};

struct mesh
{
	face * faces;
	Vector3D * vertices;
	Vector3D * normals;
	Vector2D * tex_coords;
};

struct anim_desc
{
    int no_vertices;
	float time_value;
};

struct anim
{
    Vector3D * vertices;
	Vector3D * normals;
};

class TmdModel
{
public:
    model_header desc;
	anim_header a_desc;
	material * materials;
	mesh_desc * m_descs;
    mesh * meshes;
	anim_desc * a_descs;
	anim * anims;
	int * textureIDs;

	TmdModel()
	{
        sprintf(desc.version, MODEL_VERSION);
		desc.no_meshes = 0;
		desc.no_materials = 0;
		a_desc.no_anims = 0;
		a_desc.end_time = 0.0f;
		materials = NULL;
		textureIDs = NULL;
		m_descs = NULL;
		meshes = NULL;
		a_descs = NULL;
		anims = NULL;
	};
	void Destroy();
	~TmdModel() { Destroy(); };
	
	void Load(const char * file);
};
