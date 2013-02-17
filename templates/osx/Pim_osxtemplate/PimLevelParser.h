#pragma once

#include "PimInternal.h"
#include "tinyxml.h"
#include "PimVec2.h"

namespace Pim {
	class SpriteBatchNode;
	class GameNode;
	class Sprite;
	class Layer;
	class Vec2;
	struct LightDef;
	struct Color;

	typedef vector<vector<Vec2>> Polygons;

	struct LevelData {
		Layer*							layer;
		Polygons						physics;
		Polygons						shadows;
	};


	/*
		The LevelParser object parses an XML-file, and returns a LevelData object containing
		all data defined within the file. It is designed to quickly parse a level by default
		layout, but in most cases you will require some level of customization.
		The virtual method "parseCustom(TiXmlElement*,GameNode*)" will hopefully provide
		the level of customization you need. parseCustom() is called once for every node
		being parsed - in other words, it must be a child of an XML-element.
	*/
	class LevelParser {
	public:
		LevelData						data;

		string							GetResourcePath(const string path);
		void							SetResourcePath(const string file, 
														const string resPath);
		bool							Parse(const string path, Layer* layer);

	protected:
		void							ParsePoly(TiXmlDocument *elem);
		void							ParseRootBatchNodes(TiXmlDocument *doc);
		void							ParseNode(TiXmlElement *elem, GameNode *node);
		virtual void					ParseCustom(TiXmlElement *elem, GameNode *parent) {}
		void							SetNodeAttributes(TiXmlElement *elem, GameNode *node);
		void							SetLayerAttributes(TiXmlElement *elem, Layer *layer);
		void							SetSpriteAttributes(TiXmlElement *elem, Sprite *sprite);
		Vec2							VecFromString(const char *str);
		Color							ColorFromString(const char *str);
		Rect							RectFromString(const char *str);

		/*
    		COMMON ATTRIBUTES
		*/
		void							ParsePosition(TiXmlElement *elem, GameNode *node);
		void							ParseRotation(TiXmlElement *elem, GameNode *node);
		void							ParseIdentifier(TiXmlElement *elem, GameNode *node);

		/*
    		SPRITE ATTRIBUTES
		*/
		void							ParseAnchor(TiXmlElement *elem, Sprite *sprite);
		void							ParseImage(TiXmlElement *elem, Sprite *sprite);
		void							ParseColor(TiXmlElement *elem, Sprite *sprite);
		void							ParseScale(TiXmlElement *elem, Sprite *sprite);
		void							ParseRect(TiXmlElement *elem, Sprite *sprite);
		void							ParseBatch(TiXmlElement *elem, Sprite *sprite);

		/* 
			SPRITE AND GAMENODE ATTRIBUTES 
		*/
		void							ParseLight(TiXmlElement *elem, GameNode *node);
		void							ParseLightRadius(TiXmlElement *elem, LightDef *ldef);
		void							ParseLightInnerColor(TiXmlElement *elem, LightDef *ldef);
		void							ParseLightOuterColor(TiXmlElement *elem, LightDef *ldef);
		void							ParseLightFalloff(TiXmlElement *elem, LightDef *ldef);
		void							ParseLightPosition(TiXmlElement *elem, LightDef *ldef);

		/*
    		LAYER ATTRIBUTES
		*/
		void							ParseColor(TiXmlElement *elem, Layer *layer);
		void							ParseImmovable(TiXmlElement *elem, Layer *layer);
		void							ParseScale(TiXmlElement *elem, Layer *layer);
		void							ParseLightingSystem(TiXmlElement *elem, Layer *layer);
		void							ParseLSUnlitColor(TiXmlElement *elem, Layer *layer);
		void							ParseLSLightAlpha(TiXmlElement *elem, Layer *layer);
		void							ParseLSCastShadows(TiXmlElement *elem, Layer *layer);
		void							ParseLSSmoothShadows(TiXmlElement *elem, Layer *layer);

	private:
		string							resPath;
		map<string,SpriteBatchNode*>	batchNodes;
	};
}