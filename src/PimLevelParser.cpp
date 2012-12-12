#include "PimLevelParser.h"
#include "PimAssert.h"

#include "PimLayer.h"
#include "PimGameNode.h"
#include "PimSpriteBatchNode.h"
#include "PimSprite.h"
#include "PimVec2.h"
#include "PimLightDef.h"

namespace Pim
{	
	// --------------------------------------------------------------


	std::string LevelParser::getResourcePath(std::string path)
	{
		TiXmlDocument doc( path.c_str() );
		if (!doc.LoadFile())
		{
			std::string desc = "Could not open file for parsing:\n";
			desc.append(path);
			MessageBoxA(NULL, desc.c_str(), "Error!", MB_OK | MB_ICONEXCLAMATION);
			return NULL;
		}

		TiXmlElement *root = NULL;
		root = doc.FirstChildElement("meta");
	
		if (root)
		{
			resPath = root->Attribute("res", (double*)NULL);
		}

		return resPath;
	}
	void LevelParser::setResourcePath(std::string file, std::string path)
	{
		TiXmlDocument doc( file.c_str() );
		if (!doc.LoadFile())
		{
			std::string desc = "Could not open file for writing:\n";
			desc.append(file);
			MessageBoxA(NULL, desc.c_str(), "Error!", MB_OK | MB_ICONEXCLAMATION);
			return;
		}

		TiXmlElement *root = NULL;
		root = doc.FirstChildElement("meta");
		if (root)
		{
			root->SetAttribute("res", path.c_str());
			resPath = path;
			doc.SaveFile();
		}
	}

	bool LevelParser::parse(std::string path, Layer *layer)
	{
		PimAssert(layer != NULL, "Error: must pass a non-nil layer to LevelParser::parse()!");

#ifdef PIMEDIT
		resPath				= getResourcePath(path);
#endif

		TiXmlDocument doc( path.c_str() );
		if (!doc.LoadFile())
		{
			std::string desc = "Could not open file for parsing:\n";
			desc.append(path);
			MessageBoxA(NULL, desc.c_str(), "Parser error", MB_OK | MB_ICONEXCLAMATION);
			return false;
		}

		TiXmlElement *root;
		root = doc.FirstChildElement("layer");
		if (root)
		{
			data.layer = layer;
			data.layer->identifier = "layer";

			// Parse all childless batch nodes (defined at root level in the XML)
			parseRootBatchNodes(&doc);

			// Parse polygons
			parsePoly(&doc);
		
			setLayerAttributes(root, data.layer);
			parseNode(root, data.layer);
		}
		else
		{
			return false;
		}

		return true;
	}
	

	// --------------------------------------------------------------


	void LevelParser::parsePoly(TiXmlDocument *doc)
	{
		TiXmlElement *root;

		// Parse the physics vertices
		root = doc->FirstChildElement("physic");
		if (root)
		{
			TiXmlElement *cur;
			for (cur = root->FirstChildElement("poly"); cur; cur = cur->NextSiblingElement("poly"))
			{
				const char *attr;
				std::vector<Vec2> vec;

				attr = cur->Attribute("p1", (double*)NULL);
				if (attr)
					vec.push_back(vecFromString(attr));
			
				attr = cur->Attribute("p2", (double*)NULL);
				if (attr)
					vec.push_back(vecFromString(attr));

				attr = cur->Attribute("p3", (double*)NULL);
				if (attr)
					vec.push_back(vecFromString(attr));

				data.physics.push_back(vec);
			}
		}

		// Parse the shadow-vertices
		root = doc->FirstChildElement("shadows");
		if (root)
		{
			TiXmlElement *cur;
			for (cur = root->FirstChildElement("poly"); cur; cur = cur->NextSiblingElement("poly"))
			{
				const char *attr;
				std::vector<Vec2> vec;

				attr = cur->Attribute("p1", (double*)NULL);
				if (attr)
					vec.push_back(vecFromString(attr));
			
				attr = cur->Attribute("p2", (double*)NULL);
				if (attr)
					vec.push_back(vecFromString(attr));

				attr = cur->Attribute("p3", (double*)NULL);
				if (attr)
					vec.push_back(vecFromString(attr));

				data.shadows.push_back(vec);
			}
		}
	}
	void LevelParser::parseRootBatchNodes(TiXmlDocument *doc)
	{
		TiXmlElement *cur;

		// Batch nodes
		for (cur = doc->FirstChildElement("batchnode"); cur; cur = cur->NextSiblingElement("batchnode"))
		{
			SpriteBatchNode *sbn = new SpriteBatchNode;
			data.layer->addChild(sbn);

			parseIdentifier(cur, sbn);
			parseImage(cur, sbn);

			batchNodes[sbn->identifier] = sbn;
		}
	}
	void LevelParser::parseNode(TiXmlElement *elem, GameNode *node)
	{
		TiXmlElement *cur;

		// Custom
		parseCustom(elem, node);

		// Nodes
		for (cur = elem->FirstChildElement("node"); cur; cur = cur->NextSiblingElement("node"))
		{
			GameNode *child = new GameNode;
			node->addChild(child);

			setNodeAttributes(cur, node);
			parseNode(cur, node);
		}

		// Sprites
		for (cur = elem->FirstChildElement("sprite"); cur; cur = cur->NextSiblingElement("sprite"))
		{
			Sprite *sprite = new Sprite;
			node->addChild(sprite);

			setSpriteAttributes(cur, sprite);
			parseNode(cur, sprite);
		}

		// Layers
		for (cur = elem->FirstChildElement("layer"); cur; cur->NextSiblingElement("layer"))
		{
			Layer *layer = new Layer;
			node->addChild(layer);

			setLayerAttributes(cur, layer);
			parseNode(cur, layer);
		}
	}


	// --------------------------------------------------------------


	void LevelParser::setNodeAttributes(TiXmlElement *elem, GameNode *node)
	{
		// Common
		parsePosition(elem, node);
		parseRotation(elem, node);
		parseIdentifier(elem, node);

		// Sprite & GameNode attribute
		parseLight(elem, node);
	}
	void LevelParser::setLayerAttributes(TiXmlElement *elem, Layer *layer)
	{
		// Common
		parsePosition(elem, layer);
		parseRotation(elem, layer);
		parseIdentifier(elem, layer);

		// Custom
		parseLightingSystem(elem, layer);
		parseColor(elem, layer);
		parseImmovable(elem, layer);
		parseScale(elem, layer);
	}
	void LevelParser::setSpriteAttributes(TiXmlElement *elem, Sprite *sprite)
	{
		// Common
		parsePosition(elem, sprite);
		parseRotation(elem, sprite);
		parseIdentifier(elem, sprite);

		// Sprite & GameNode attribute
		parseLight(elem, sprite);
	
		// Custom for sprites
		parseAnchor(elem, sprite);
		parseImage(elem, sprite);
		parseColor(elem, sprite);
		parseScale(elem, sprite);
		parseRect(elem, sprite);
		parseBatch(elem, sprite);
	}


	// --------------------------------------------------------------


	Vec2 LevelParser::vecFromString(const char *str)
	{
		const char *x = str;
		const char *y = str;

		for (int i=0; i<strlen(str); i++)
		{
			if (str[i] == ' ')
				y = str+i;
		}

		return Vec2( 
			(float)atof(x), 
			(float)atof(y) 
		);
	}
	Color LevelParser::colorFromString(const char *str)
	{
		const char *rgba[4] = {str};

		int idx = 1;
		for (int i=0; i<strlen(str); i++)
		{
			if (str[i] == ' ')
				rgba[idx++] = str+i;
		}

		return Color(
			(float)atof(rgba[0]),
			(float)atof(rgba[1]),
			(float)atof(rgba[2]),
			(float)atof(rgba[3])
		);
	}
	Rect LevelParser::rectFromString(const char *str)
	{
		// Rect and color are both just 4D vectors.
		Color c = colorFromString(str);
		return Rect(c.r, c.g, c.b, c.a);
	}


	// --------------------------------------------------------------


	/* COMMON */
	void LevelParser::parsePosition(TiXmlElement *elem, GameNode *node)
	{
		const char *attr = elem->Attribute("position", (double*)NULL);

		if (attr != NULL)
		{
			node->position = vecFromString(attr);
		}
	}
	void LevelParser::parseRotation(TiXmlElement *elem, GameNode *node)
	{
		double attr = 0;
		elem->Attribute("rotation", &attr);

		if (attr != NULL)
		{
			node->rotation = attr;
		}
	}
	void LevelParser::parseIdentifier(TiXmlElement *elem, GameNode *node)
	{
		const char *attr = elem->Attribute("identifier", (double*)NULL);
		
		if (attr != NULL)
		{
			node->identifier = attr;
		}
	}

	/* SPRITE */
	void LevelParser::parseAnchor(TiXmlElement *elem, Sprite *sprite)
	{
		const char *attr = elem->Attribute("anchor", (double*)NULL);
	
		if (attr != NULL)
		{
			sprite->anchor = vecFromString(attr);
		}
	}
	void LevelParser::parseImage(TiXmlElement *elem, Sprite *sprite)
	{
		const char *attr = elem->Attribute("img", (double*)NULL);

		if (attr != NULL)
		{
			std::string res;

			if (attr[0] != 'C')
			{
				res = resPath;
				res.append(attr);
			}
			else
			{
				res = attr;
			}

			sprite->loadSprite(res);
		}
	}
	void LevelParser::parseColor(TiXmlElement *elem, Sprite *sprite)
	{
		const char *attr = elem->Attribute("color", (double*)NULL);

		if (attr != NULL)
		{
			sprite->color = colorFromString(attr);
		}
	}
	void LevelParser::parseScale(TiXmlElement *elem, Sprite *sprite)
	{
		const char *attr = elem->Attribute("scale", (double*)NULL);

		if (attr != NULL)
		{
			sprite->scale = vecFromString(attr);
		}
	}
	void LevelParser::parseRect(TiXmlElement *elem, Sprite *sprite)
	{
		const char *attr = elem->Attribute("rect", (double*)NULL);

		if (attr != NULL)
		{
			sprite->rect = rectFromString(attr);
		}
	}
	void LevelParser::parseBatch(TiXmlElement *elem, Sprite *sprite)
	{
		const char *attr = elem->Attribute("batchnode", (double*)NULL);

		if (attr != NULL)
		{
			if (batchNodes.count(attr))
			{
				sprite->useBatchNode(batchNodes[attr]);
			}
			else
			{
				char *desc = "ERROR:\nReferences batchnode does not exist:\n";
				strcat(desc, attr);
				MessageBoxA(NULL, desc, "Parser error", MB_OK | MB_ICONEXCLAMATION);
			}
		}
	}

	/* SPRITE AND GAMENODE */
	void LevelParser::parseLight(TiXmlElement *elem, GameNode *node)
	{
		TiXmlElement *light = elem->FirstChildElement("light");
		
		if (light)
		{
			LightDef *ldef;

			const char *type = light->Attribute("type", (double*)NULL);
			if (type != NULL)
			{
				if (!strcmp(type, "smooth"))
				{
					ldef = new SmoothLightDef;	
				}
				else if (!strcmp(type, "flat"))
				{
					ldef = new FlatLightDef;
				}
				else
				{
					MessageBoxA(NULL,
						"Error:\nBad type specified (flat/smooth) for light.",
						"Parser error", MB_OK | MB_ICONEXCLAMATION
					);
					return;
				}
			}
			else
			{
				MessageBoxA(NULL,
					"Error:\nBad type specified (flat/smooth) for light.",
					"Parser error", MB_OK | MB_ICONEXCLAMATION
				);
				return;
			}

			parseLightRadius(light, ldef);
			parseLightInnerColor(light, ldef);
			parseLightOuterColor(light, ldef);
			parseLightFalloff(light, ldef);
			parseLightPosition(light, ldef);

			node->getParentLayer()->addLight(node, ldef);
		}
	}
	void LevelParser::parseLightRadius(TiXmlElement *elem, LightDef *ldef)
	{
		const char *attr = elem->Attribute("radius", (double*)NULL);
		
		if (attr != NULL)
		{
			ldef->radius = atof(attr);
		}
	}
	void LevelParser::parseLightInnerColor(TiXmlElement *elem, LightDef *ldef)
	{
		const char *attr = elem->Attribute("innercolor", (double*)NULL);

		if (attr != NULL)
		{
			ldef->innerColor = colorFromString(attr);
		}
	}
	void LevelParser::parseLightOuterColor(TiXmlElement *elem, LightDef *ldef)
	{
		const char *attr = elem->Attribute("outercolor", (double*)NULL);

		if (attr != NULL)
		{
			ldef->outerColor = colorFromString(attr);
		}
	}
	void LevelParser::parseLightFalloff(TiXmlElement *elem, LightDef *ldef)
	{
		const char *attr = elem->Attribute("falloff", (double*)NULL);
		
		if (attr != NULL)
		{
			ldef->falloff = atof(attr);
		}
	}
	void LevelParser::parseLightPosition(TiXmlElement *elem, LightDef *ldef)
	{
		const char *attr = elem->Attribute("position", (double*)NULL);
		
		if (attr != NULL)
		{
			ldef->position = vecFromString(attr);
		}
	}

	/* LAYER */
	void LevelParser::parseColor(TiXmlElement *elem, Layer *layer)
	{
		const char *attr = elem->Attribute("color", (double*)NULL);

		if (attr != NULL)
		{
			layer->color = colorFromString(attr);
		}
	}
	void LevelParser::parseImmovable(TiXmlElement *elem, Layer *layer)
	{
		double attr = 0;
		elem->Attribute("immovable", &attr);

		if (attr != 0)
		{
			layer->immovable = (bool)attr;
		}
	}
	void LevelParser::parseScale(TiXmlElement *elem, Layer *layer)
	{
		const char *attr = elem->Attribute("scale", (double*)NULL);

		if (attr != NULL)
		{
			layer->scale = vecFromString(attr);
		}
	}

	void LevelParser::parseLightingSystem(TiXmlElement *elem, Layer *layer)
	{
		TiXmlElement *lightSys = elem->FirstChildElement("lightsys");

		if (lightSys)
		{
			Vec2 resolution(800.f, 600.f); 

			const char *attr = lightSys->Attribute("resolution", (double*)NULL);
			if (attr != NULL)
			{
				resolution = vecFromString(attr);
			}
#ifdef _DEBUG
			else
			{
				// Show a warning message
				MessageBoxA(NULL, 
					"ERROR:\nA lighting system was defined, but no resolution was given.", 
					"Level Error", MB_OK | MB_ICONEXCLAMATION
				);
			}
#endif

			layer->createLightingSystem(resolution);

			parseLSUnlitColor(lightSys, layer);
			parseLSLightAlpha(lightSys, layer);
			parseLSCastShadows(lightSys, layer);
			parseLSSmoothShadows(lightSys, layer);
		}
	}
	void LevelParser::parseLSUnlitColor(TiXmlElement *elem, Layer *layer)
	{
		const char *attr = elem->Attribute("color", (double*)NULL);
		if (attr != NULL)
		{
			layer->setLightingUnlitColor(colorFromString(attr));
		}
	}
	void LevelParser::parseLSLightAlpha(TiXmlElement *elem, Layer *layer)
	{
		const char *attr = elem->Attribute("lightalpha", (double*)NULL);
		if (attr != NULL)
		{
			layer->setLightAlpha(atof(attr));
		}
	}
	void LevelParser::parseLSCastShadows(TiXmlElement *elem, Layer *layer)
	{
		const char *attr = elem->Attribute("castshadows", (double*)NULL);
		if (attr != NULL)
		{
			layer->setCastShadows( (bool)atoi(attr) );
		}
	}
	void LevelParser::parseLSSmoothShadows(TiXmlElement *elem, Layer *layer)
	{
		const char *attr = elem->Attribute("smoothshadows", (double*)NULL);
		if (attr != NULL)
		{
			layer->setSmoothShadows( (bool)atoi(attr) );
		}
	}
}