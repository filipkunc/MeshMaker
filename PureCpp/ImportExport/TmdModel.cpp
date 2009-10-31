//
//  TmdModel.cpp
//  Original source - RT3D Engine 2.0
//
//  Created by Filip Kunc on 10/18/09.
//  For license see LICENSE.TXT
//

#include <fstream>
using namespace std;

#include "TmdModel.h"

void TmdModel::Load(const char * file)
{
	Destroy();
    ifstream fin(file, ios::in | ios::binary);
	if (!fin.is_open())
	{
		fin.close();
		return;
	}
	fin.read((char *) &desc, sizeof desc);
#ifdef WIN32
	if (strcmpi(desc.version, MODEL_VERSION) != 0)
#else
	if (strcasecmp(desc.version, MODEL_VERSION) != 0)
#endif
	{
		desc.no_materials = 0;
		desc.no_meshes = 0;
        return;
	}
	fin.read((char *) &a_desc, sizeof a_desc);
	materials = new material[desc.no_materials];
	textureIDs = new int[desc.no_materials];
	m_descs = new mesh_desc[desc.no_meshes];
	meshes = new mesh[desc.no_meshes];
	a_descs = new anim_desc[a_desc.no_anims];
	anims = new anim[a_desc.no_anims];
	int i, j;
	for (i = 0; i < desc.no_materials; i++)
	{
        fin.read((char *) &materials[i], sizeof materials[i]);
		textureIDs[i] = 0;
	}
	for (i = 0; i < desc.no_meshes; i++)
	{
        fin.read((char *) &m_descs[i], sizeof m_descs[i]);
		meshes[i].faces = new face[m_descs[i].no_faces];
	    for (j = 0; j < m_descs[i].no_faces; j++)
	    {
            fin.read((char *) &meshes[i].faces[j], sizeof meshes[i].faces[j]);
	    }
		meshes[i].vertices = new Vector3D[m_descs[i].no_vertices];
		meshes[i].normals = new Vector3D[m_descs[i].no_vertices];
		meshes[i].tex_coords = new Vector2D[m_descs[i].no_texcoords];
	    for (j = 0; j < m_descs[i].no_vertices; j++)
	    {
            fin.read((char *) &meshes[i].vertices[j], sizeof meshes[i].vertices[j]);
	        fin.read((char *) &meshes[i].normals[j], sizeof meshes[i].normals[j]);
	    }
		for (j = 0; j < m_descs[i].no_texcoords; j++)
	    {
			fin.read((char *) &meshes[i].tex_coords[j], sizeof meshes[i].tex_coords[j]);
		}
    }
	for (i = 0; i < a_desc.no_anims; i++)
	{
		fin.read((char *) &a_descs[i], sizeof a_descs[i]);
		anims[i].vertices = new Vector3D [a_descs[i].no_vertices];
		anims[i].normals = new Vector3D [a_descs[i].no_vertices];
		for (j = 0; j < a_descs[i].no_vertices; j++)
	    {
            fin.read((char *) &anims[i].vertices[j], sizeof anims[i].vertices[j]);
			fin.read((char *) &anims[i].normals[j], sizeof anims[i].normals[j]);
		}
	}
	fin.close();
}

void TmdModel::Destroy()
{
	int i;
	if (materials != NULL)
	{
		delete [] materials;
		materials = NULL;
	}
	if (textureIDs != NULL)
	{
        delete [] textureIDs;
		textureIDs = NULL;
	}
	if (m_descs != NULL)
	{
        delete [] m_descs;
		m_descs = NULL;
	}
	if (meshes != NULL)
	{
        for (i = 0; i < desc.no_meshes; i++)
		{
            delete [] meshes[i].faces;
			delete [] meshes[i].vertices;
			delete [] meshes[i].normals;
			delete [] meshes[i].tex_coords;
		}
		delete [] meshes;
		meshes = NULL;
	}
	if (a_descs != NULL)
	{
        delete [] a_descs;
		a_descs = NULL;
	}
	if (anims != NULL)
	{
		for (i = 0; i < a_desc.no_anims; i++)
		{
			delete [] anims[i].vertices;
			delete [] anims[i].normals;
		}
		delete [] anims;
		anims = NULL;
	}
	sprintf(desc.version, MODEL_VERSION);
	desc.no_meshes = 0;
	desc.no_materials = 0;
	a_desc.no_anims = 0;
	a_desc.end_time = 0.0f;
}