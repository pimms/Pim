#include "PimLevelParser.h"
#include "PimAssert.h"

#include "PimLayer.h"
#include "PimGameNode.h"
#include "PimSpriteBatchNode.h"
#include "PimSprite.h"
#include "PimVec2.h"
#include "PimLightDef.h"

namespace Pim {
	/*
	=====================
	LevelParser::GetResourcePath
	=====================
	*/
	string LevelParser::GetResourcePath(const string path) {
		TiXmlDocument doc(path.c_str());
		if (!doc.LoadFile()) {
			string desc = "Could not open file for parsing:\n";
			desc.append(path);
			PimWarning(desc.c_str(), "Error!");
			return NULL;
		}

		TiXmlElement *root = NULL;
		root = doc.FirstChildElement("meta");

		if (root) {
			resPath = root->Attribute("res", (double*)NULL);
		}

		return resPath;
	}

	/*
	=====================
	LevelParser::SetResourcePath
	=====================
	*/
	void LevelParser::SetResourcePath(const string file, const string path) {
		TiXmlDocument doc(file.c_str());
		if (!doc.LoadFile()) {
			string desc = "Could not open file for writing:\n";
			desc.append(file);
			PimWarning(desc.c_str(), "Error!");
			return;
		}

		TiXmlElement *root = NULL;
		root = doc.FirstChildElement("meta");
		if (root) {
			root->SetAttribute("res", path.c_str());
			resPath = path;
			doc.SaveFile();
		}
	}

	/*
	=====================
	LevelParser::Parse
	=====================
	*/
	bool LevelParser::Parse(const string path, Layer *layer) {
		PimAssert(layer != NULL, "Error: must pass a non-nil layer to LevelParser::parse()!");

	#ifdef PIMEDIT
		resPath	= GetResourcePath(path);
	#endif

		TiXmlDocument doc( path.c_str() );
		if (!doc.LoadFile()) {
			string desc = "Could not open file for parsing:\n";
			desc.append(path);
			PimWarning(desc.c_str(), "Parser error");
			return false;
		}

		TiXmlElement *root;
		root = doc.FirstChildElement("layer");
		if (root) {
			data.layer = layer;
			data.layer->identifier = "layer";

			// Parse all childless batch nodes (defined at root level in the XML)
			ParseRootBatchNodes(&doc);

			// Parse polygons
			ParsePoly(&doc);

			SetLayerAttributes(root, data.layer);
			ParseNode(root, data.layer);
		} else {
			return false;
		}

		return true;
	}


	/*
	=====================
	LevelParser::ParsePoly
	=====================
	*/
	void LevelParser::ParsePoly(TiXmlDocument *doc) {
		TiXmlElement *root = NULL;

		// Parse the physics vertices
		root = doc->FirstChildElement("physic");
		if (root) {
			TiXmlElement *cur;
			for (cur = root->FirstChildElement("poly"); cur; cur = cur->NextSiblingElement("poly")) {
				const char *attr;
				vector<Vec2> vec;

				attr = cur->Attribute("p1");
				if (attr) {
					vec.push_back(VecFromString(attr));
				}

				attr = cur->Attribute("p2");
				if (attr) {
					vec.push_back(VecFromString(attr));
				}

				attr = cur->Attribute("p3");
				if (attr) {
					vec.push_back(VecFromString(attr));
				}

				data.physics.push_back(vec);
			}
		}

		// Parse the shadow-vertices
		root = doc->FirstChildElement("shadows");
		if (root) {
			TiXmlElement *cur;
			for (cur = root->FirstChildElement("poly"); cur; cur = cur->NextSiblingElement("poly")) {
				const char *attr;
				vector<Vec2> vec;

				attr = cur->Attribute("p1");
				if (attr) {
					vec.push_back(VecFromString(attr));
				}

				attr = cur->Attribute("p2");
				if (attr) {
					vec.push_back(VecFromString(attr));
				}

				attr = cur->Attribute("p3");
				if (attr) {					
					vec.push_back(VecFromString(attr));
				}

				data.shadows.push_back(vec);
			}
		}
	}

	/*
	=====================
	LevelParser::ParseRootBatchNodes
	=====================
	*/
	void LevelParser::ParseRootBatchNodes(TiXmlDocument *doc) {
		TiXmlElement *cur;

		// Batch nodes
		for (cur = doc->FirstChildElement("batchnode"); cur; 
			cur = cur->NextSiblingElement("batchnode")) {
			SpriteBatchNode *sbn = new SpriteBatchNode;
			data.layer->AddChild(sbn);

			ParseIdentifier(cur, sbn);
			ParseImage(cur, sbn);

			batchNodes[sbn->identifier] = sbn;
		}
	}

	/*
	=====================
	LevelParser::ParseNode
	=====================
	*/
	void LevelParser::ParseNode(TiXmlElement *elem, GameNode *node) {
		TiXmlElement *cur;

		// Custom
		ParseCustom(elem, node);

		// Nodes
		for (cur = elem->FirstChildElement("node"); cur; cur = cur->NextSiblingElement("node")) {
			GameNode *child = new GameNode;
			node->AddChild(child);

			SetNodeAttributes(cur, node);
			ParseNode(cur, node);
		}

		// Sprites
		for (cur = elem->FirstChildElement("sprite"); cur; cur = cur->NextSiblingElement("sprite")) {
			Sprite *sprite = new Sprite;
			node->AddChild(sprite);

			SetSpriteAttributes(cur, sprite);
			ParseNode(cur, sprite);
		}

		// Layers
		for (cur = elem->FirstChildElement("layer"); cur; cur->NextSiblingElement("layer")) {
			Layer *layer = new Layer;
			node->AddChild(layer);

			SetLayerAttributes(cur, layer);
			ParseNode(cur, layer);
		}
	}



	/*
	=====================
	LevelParser::SetNodeAttributes
	=====================
	*/
	void LevelParser::SetNodeAttributes(TiXmlElement *elem, GameNode *node) {
		// Common
		ParsePosition(elem, node);
		ParseRotation(elem, node);
		ParseIdentifier(elem, node);

		// Sprite & GameNode attribute
		ParseLight(elem, node);
	}

	/*
	=====================
	LevelParser::SetLayerAttributes
	=====================
	*/
	void LevelParser::SetLayerAttributes(TiXmlElement *elem, Layer *layer) {
		// Common
		ParsePosition(elem, layer);
		ParseRotation(elem, layer);
		ParseIdentifier(elem, layer);

		// Custom
		ParseLightingSystem(elem, layer);
		ParseColor(elem, layer);
		ParseImmovable(elem, layer);
		ParseScale(elem, layer);
	}

	/*
	=====================
	LevelParser::SetSpriteAttributes
	=====================
	*/
	void LevelParser::SetSpriteAttributes(TiXmlElement *elem, Sprite *sprite) {
		// Common
		ParsePosition(elem, sprite);
		ParseRotation(elem, sprite);
		ParseIdentifier(elem, sprite);
	
		// Sprite & GameNode attribute
		ParseLight(elem, sprite);

		// Custom for sprites
		ParseAnchor(elem, sprite);
		ParseImage(elem, sprite);
		ParseColor(elem, sprite);
		ParseScale(elem, sprite);
		ParseRect(elem, sprite);
		ParseBatch(elem, sprite);
	}



	/*
	=====================
	LevelParser::VecFromString
	=====================
	*/
	Vec2 LevelParser::VecFromString(const char *str) {
		const char *x = str;
		const char *y = str;

		for (unsigned i=0; i<strlen(str); i++) {
			if (str[i] == ' ') {
				y = str+i;
			}
		}

		return Vec2(
			(float)atof(x),
			(float)atof(y)
		);
	}

	/*
	=====================
	LevelParser::ColorFromString
	=====================
	*/
	Color LevelParser::ColorFromString(const char *str) {
		const char *rgba[4] = {str};

		int idx = 1;
		for (unsigned i=0; i<strlen(str); i++) {
			if (str[i] == ' ') {
				rgba[idx++] = str+i;
			}
		}

		return Color(
			(float)atof(rgba[0]),
			(float)atof(rgba[1]),
			(float)atof(rgba[2]),
			(float)atof(rgba[3])
		);
	}

	/*
	=====================
	LevelParser::RectFromString
	=====================
	*/
	Rect LevelParser::RectFromString(const char *str) {
		// Rect and color are both just 4D vectors.
		Color c = ColorFromString(str);
		return Rect(c.r, c.g, c.b, c.a);
	}



	/* COMMON */

	/*
	=====================
	LevelParser::ParsePosition
	=====================
	*/
	void LevelParser::ParsePosition(TiXmlElement *elem, GameNode *node) {
		const char *attr = elem->Attribute("position", (double*)NULL);

		if (attr != NULL) {
			node->position = VecFromString(attr);
		}
	}

	/*
	=====================
	LevelParser::ParseRotation
	=====================
	*/
	void LevelParser::ParseRotation(TiXmlElement *elem, GameNode *node) {
		double attr = 0;
		elem->Attribute("rotation", &attr);

		if (attr != 0.0) {
			node->rotation = float(attr);
		}
	}

	/*
	=====================
	LevelParser::ParseIdentifier
	=====================
	*/
	void LevelParser::ParseIdentifier(TiXmlElement *elem, GameNode *node) {
		const char *attr = elem->Attribute("identifier");

		if (attr != NULL) {
			node->identifier = attr;
		}
	}


	/* SPRITE */

	/*
	=====================
	LevelParser::ParseAnchor
	=====================
	*/
	void LevelParser::ParseAnchor(TiXmlElement *elem, Sprite *sprite) {
		const char *attr = elem->Attribute("anchor");

		if (attr != NULL) {
			sprite->anchor = VecFromString(attr);
		}
	}

	/*
	=====================
	LevelParser::ParseImage
	=====================
	*/
	void LevelParser::ParseImage(TiXmlElement *elem, Sprite *sprite) {
		const char *attr = elem->Attribute("img");

		if (attr != NULL) {
			string res;

			if (attr[0] != 'C') {
				res = resPath;
				res.append(attr);
			} else {
				res = attr;
			}

			sprite->LoadSprite(res);
		}
	}

	/*
	=====================
	LevelParser::ParseColor
	=====================
	*/
	void LevelParser::ParseColor(TiXmlElement *elem, Sprite *sprite) {
		const char *attr = elem->Attribute("color");

		if (attr != NULL) {
			sprite->color = ColorFromString(attr);
		}
	}

	/*
	=====================
	LevelParser::ParseScale
	=====================
	*/
	void LevelParser::ParseScale(TiXmlElement *elem, Sprite *sprite) {
		const char *attr = elem->Attribute("scale");

		if (attr != NULL) {
			sprite->scale = VecFromString(attr);
		}
	}

	/*
	=====================
	LevelParser::ParseRect
	=====================
	*/
	void LevelParser::ParseRect(TiXmlElement *elem, Sprite *sprite) {
		const char *attr = elem->Attribute("rect");

		if (attr != NULL) {
			sprite->rect = RectFromString(attr);
		}
	}

	/*
	=====================
	LevelParser::ParseBatch
	=====================
	*/
	void LevelParser::ParseBatch(TiXmlElement *elem, Sprite *sprite) {
		const char *attr = elem->Attribute("batchnode");

		if (attr != NULL) {
			if (batchNodes.count(attr)) {
				sprite->UseBatchNode(batchNodes[attr]);
			} else {
				
				string desc = "ERROR:\nReferences batchnode does not exist:\n";
				desc.append(attr);
				PimWarning(desc.c_str(), "Parser error");
			}
		}
	}


	/* SPRITE AND GAMENODE */

	/*
	=====================
	LevelParser::ParseLight
	=====================
	*/
	void LevelParser::ParseLight(TiXmlElement *elem, GameNode *node) {
		TiXmlElement *light = elem->FirstChildElement("light");

		if (light) {
			LightDef *ldef;

			const char *type = light->Attribute("type");
			if (type != NULL) {
				if (!strcmp(type, "smooth")) {
					ldef = new SmoothLightDef;
				} else if (!strcmp(type, "flat")) {
					ldef = new FlatLightDef;
				} else {
					PimWarning("Error:\nBad type specified (flat/smooth) for light.",
							   "Parser error");
					return;
				}
			} else {
				PimWarning("Error:\nBad type specified (flat/smooth) for light.",
						   "Parser error");
				return;
			}

			ParseLightRadius(light, ldef);
			ParseLightInnerColor(light, ldef);
			ParseLightOuterColor(light, ldef);
			ParseLightFalloff(light, ldef);
			ParseLightPosition(light, ldef);

			node->GetParentLayer()->AddLight(node, ldef);
		}
	}

	/*
	=====================
	LevelParser::ParseLightRadius
	=====================
	*/
	void LevelParser::ParseLightRadius(TiXmlElement *elem, LightDef *ldef) {
		const char *attr = elem->Attribute("radius");

		if (attr != NULL) {
			ldef->radius = (float)atof(attr);
		}
	}

	/*
	=====================
	LevelParser::ParseLightInnerColor
	=====================
	*/
	void LevelParser::ParseLightInnerColor(TiXmlElement *elem, LightDef *ldef) {
		const char *attr = elem->Attribute("innercolor");

		if (attr != NULL) {
			ldef->innerColor = ColorFromString(attr);
		}
	}

	/*
	=====================
	LevelParser::ParseLightOuterColor
	=====================
	*/
	void LevelParser::ParseLightOuterColor(TiXmlElement *elem, LightDef *ldef) {
		const char *attr = elem->Attribute("outercolor");

		if (attr != NULL) {
			ldef->outerColor = ColorFromString(attr);
		}
	}
	
	/*
	=====================
	LevelParser::ParseLightFalloff
	=====================
	*/
	void LevelParser::ParseLightFalloff(TiXmlElement *elem, LightDef *ldef) {
		const char *attr = elem->Attribute("falloff");

		if (attr != NULL) {
			ldef->falloff = (float)atof(attr);
		}
	}
	
	/*
	=====================
	LevelParser::ParseLightPosition
	=====================
	*/
	void LevelParser::ParseLightPosition(TiXmlElement *elem, LightDef *ldef) {
		const char *attr = elem->Attribute("position");

		if (attr != NULL) {
			ldef->position = VecFromString(attr);
		}
	}


	/* LAYER */
	

	/*
	=====================
	LevelParser::ParseColor
	=====================
	*/
	void LevelParser::ParseColor(TiXmlElement *elem, Layer *layer) {
		const char *attr = elem->Attribute("color");

		if (attr != NULL) {
			layer->color = ColorFromString(attr);
		}
	}

	/*
	=====================
	LevelParser::ParseImmovable
	=====================
	*/
	void LevelParser::ParseImmovable(TiXmlElement *elem, Layer *layer) {
		double attr = 0;
		elem->Attribute("immovable", &attr);

		layer->immovable = (attr != 0.0);
	}

	/*
	=====================
	LevelParser::ParseScale
	=====================
	*/
	void LevelParser::ParseScale(TiXmlElement *elem, Layer *layer) {
		const char *attr = elem->Attribute("scale");

		if (attr != NULL) {
			layer->scale = VecFromString(attr);
		}
	}

	/*
	=====================
	LevelParser::ParseLightingSystem
	=====================
	*/
	void LevelParser::ParseLightingSystem(TiXmlElement *elem, Layer *layer) {
		TiXmlElement *lightSys = elem->FirstChildElement("lightsys");

		if (lightSys) {
			Vec2 resolution(800.f, 600.f);

			const char *attr = lightSys->Attribute("resolution");
			if (attr != NULL) {
				resolution = VecFromString(attr);
			}
#ifdef _DEBUG
			else {
				PimWarning("ERROR:\nA lighting system was defined, but no resolution was given.",
						   "Level Error");
			}
#endif

			layer->CreateLightingSystem(resolution);

			ParseLSUnlitColor(lightSys, layer);
			ParseLSLightAlpha(lightSys, layer);
			ParseLSCastShadows(lightSys, layer);
			ParseLSSmoothShadows(lightSys, layer);
		}
	}

	/*
	=====================
	LevelParser::ParseLSUnlitColor
	=====================
	*/
	void LevelParser::ParseLSUnlitColor(TiXmlElement *elem, Layer *layer) {
		const char *attr = elem->Attribute("color");

		if (attr != NULL) {
			layer->SetLightingUnlitColor(ColorFromString(attr));
		}
	}

	/*
	=====================
	LevelParser::ParseLSLightAlpha
	=====================
	*/
	void LevelParser::ParseLSLightAlpha(TiXmlElement *elem, Layer *layer) {
		const char *attr = elem->Attribute("lightalpha");

		if (attr != NULL) {
			layer->SetLightAlpha((float)atof(attr));
		}
	}
	
	/*
	=====================
	LevelParser::ParseLSCastShadows
	=====================
	*/
	void LevelParser::ParseLSCastShadows(TiXmlElement *elem, Layer *layer) {
		double attr;
		elem->Attribute("castshadows", &attr);

		layer->SetCastShadows(attr != 0.0);
	}

	/*
	=====================
	LevelParser::ParseLSSmoothShadows
	=====================
	*/
	void LevelParser::ParseLSSmoothShadows(TiXmlElement *elem, Layer *layer) {
		double attr;
		elem->Attribute("smoothshadows", &attr);

		if (attr != NULL) {
			layer->SetSmoothShadows(attr != 0.0);
		}
	}
}