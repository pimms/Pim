#include "PimScene.h"
#include "PimLayer.h"
#include "PimGameControl.h"
#include "PimRenderWindow.h"

namespace Pim {
	/*
	=====================
	Scene::Scene
	=====================
	*/
	Scene::Scene() {
		dirtyZOrder = true;
	}

	/*
	=====================
	Scene::~Scene
	=====================
	*/
	Scene::~Scene() {
		for (unsigned i=0; i<layers.size(); i++) {
			GameControl::GetSingleton()->AddNodeToDelete(layers[i]);
		}
	}

	/*
	=====================
	Scene::AddLayer
	=====================
	*/
	void Scene::AddLayer(Layer *l) {
		layers.push_back(l);
		l->parentScene = this;
		l->LoadResources();

		dirtyZOrder = true;
	}

	/*
	=====================
	Scene::RemoveLayer
	=====================
	*/
	void Scene::RemoveLayer(Layer *l) {
		for (unsigned int i=0; i<layers.size(); i++) {
			if (layers[i] == l) {
				layers.erase(layers.begin() + i);
				delete l;
			}
		}
	}

	/*
	=====================
	Scene::PauseLayer
	=====================
	*/
	Layer* Scene::PauseLayer() {
		// Return NULL by default. This will not pause the game.
		return NULL;
	}
	
	/*
	=====================
	Scene::ChildCount
	=====================
	*/
	int Scene::ChildCount() {
		int count = (int)layers.size();
		
		for (unsigned i=0; i<layers.size(); i++) {
			count += layers[i]->ChildCount();
		}
		
		return count;
	}
	
	/*
	=====================
	Scene::DrawScene
	=====================
	*/
	void Scene::DrawScene() {
		OrderLayers();

		for (unsigned int i=0; i<layers.size(); i++) {
			layers[i]->Draw();
		}
	}

	/*
	=====================
	Scene::OrderLayers
	=====================
	*/
	void Scene::OrderLayers() {
		if (!dirtyZOrder) {
			return;
		}

		// Insertion sorting - the layers should be somewhat sorted already.
		for (unsigned int j=1; j<layers.size(); j++) {
			Layer *key = layers[j];
			int i = j - 1;

			while (i >= 0 && layers[i]->zOrder < key->zOrder) {
				layers[i+1] = layers[i];
				i--;
			}
			layers[i+1] = key;
		}

		dirtyZOrder = false;
	}

	/*
	==================
	Scene::ReloadTextures
	==================
	*/
	void Scene::ReloadTextures() {
		for (unsigned i=0; i<layers.size(); i++) {
			layers[i]->ReloadTextures();
		}
	}
}