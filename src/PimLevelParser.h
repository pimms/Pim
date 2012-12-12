#pragma once

#include "PimInternal.h"
#include "tinyxml.h"

namespace Pim
{
	// Forward declarations
	class SpriteBatchNode;
	class GameNode;
	struct LightDef;
	class Sprite;
	class Layer;
	class Vec2;
	struct Color;
	struct Rect;

	typedef std::vector<std::vector<Vec2>> Polygons;

	// The level data object - contains all data found within the PIM-file
	struct LevelData
	{
		// The main layer - all nodes, sprites etc are children of this layer
		Layer		*layer;

		// Polygons flagged as physics. As there currently is no physics integrations
		// in Pim, you have to handle these polygons yourself. Manually. Alone. In the dark.
		Polygons	physics;

		// Polygons flagged as shadow caster
		Polygons	shadows;
	};


	/*
		The LevelParser object parses an XML-file, and returns a LevelData object containing
		all data defined within the file. It is designed to quickly parse a level by default
		layout, but in most cases you will require some level of customization.
		The virtual method "parseCustom(TiXmlElement*,GameNode*)" will hopefully provide
		the level of customization you need. parseCustom() is called once for every node
		being parsed - in other words, it must be a child of an XML-element.
	*/
	class LevelParser
	{
	public:

		// Returns the resource path from the meta data in the provided file
		// Primarily used by PimEdit, but it may be useful if your resources are in odd places.
		std::string getResourcePath(std::string path);

		// Changes the resource path in the meta data in the provided file
		// Also mainly used by the PimEdit app.
		void setResourcePath(std::string file, std::string resPath);

		// Parse a level file
		bool parse(std::string path, Layer *layer);

		LevelData data;

	private:

		std::string								resPath;
		std::map<std::string,SpriteBatchNode*>	batchNodes;

	protected:

		// Parse polygons
		void parsePoly(TiXmlDocument *elem);

		// parse SpriteBatchNodes on root
		void parseRootBatchNodes(TiXmlDocument *doc);

		// Handles children of the node
		void parseNode(TiXmlElement *elem, GameNode *node);

		// Custom parsing - called for every node and requires overriding
		virtual void parseCustom(TiXmlElement *elem, GameNode *parent) {}

		// Parses attributes and sets them accordingly on the passed node
		void setNodeAttributes(TiXmlElement *elem, GameNode *node);

		// Parses attributes and sets them accordingly on the passed layer
		void setLayerAttributes(TiXmlElement *elem, Layer *layer);

		// Parses attributes and sets them accordingly on the passed sprite
		void setSpriteAttributes(TiXmlElement *elem, Sprite *sprite);

	
		//////////////////////////  NOTICE  //////////////////////////////
		//		   The following methods reads a string input,          //
		//     and returns a Vec2 or a Color structure.	    //
		//////////////////////////////////////////////////////////////////

		Vec2 vecFromString(const char *str);
		Color colorFromString(const char *str);
		Rect rectFromString(const char *str);
	
	
		//////////////////////////  NOTICE  //////////////////////////////
		//		The following methods all parse the data and            //
		//	 sets an attribute accordingly. The attribute should        //
		//	     be easily identifiable by the method names.            //
		//////////////////////////////////////////////////////////////////
	
		/*
			COMMON ATTRIBUTES
		*/
		void parsePosition(TiXmlElement *elem, GameNode *node);
		void parseRotation(TiXmlElement *elem, GameNode *node);
		void parseIdentifier(TiXmlElement *elem, GameNode *node);

		/*
			SPRITE ATTRIBUTES
		*/
		void parseAnchor(TiXmlElement *elem, Sprite *sprite);
		void parseImage(TiXmlElement *elem, Sprite *sprite);
		void parseColor(TiXmlElement *elem, Sprite *sprite);
		void parseScale(TiXmlElement *elem, Sprite *sprite);
		void parseRect(TiXmlElement *elem, Sprite *sprite);
		void parseBatch(TiXmlElement *elem, Sprite *sprite);

		/* SPRITE AND GAMENODE ATTRIBUTES */
		void parseLight(TiXmlElement *elem, GameNode *node);
		void parseLightRadius(TiXmlElement *elem, LightDef *ldef);
		void parseLightInnerColor(TiXmlElement *elem, LightDef *ldef);
		void parseLightOuterColor(TiXmlElement *elem, LightDef *ldef);
		void parseLightFalloff(TiXmlElement *elem, LightDef *ldef);
		void parseLightPosition(TiXmlElement *elem, LightDef *ldef);

		/*
			LAYER ATTRIBUTES
		*/
		void parseColor(TiXmlElement *elem, Layer *layer);
		void parseImmovable(TiXmlElement *elem, Layer *layer);
		void parseScale(TiXmlElement *elem, Layer *layer);

		void parseLightingSystem(TiXmlElement *elem, Layer *layer);
		void parseLSUnlitColor(TiXmlElement *elem, Layer *layer);
		void parseLSLightAlpha(TiXmlElement *elem, Layer *layer);
		void parseLSCastShadows(TiXmlElement *elem, Layer *layer);
		void parseLSSmoothShadows(TiXmlElement *elem, Layer *layer);
	};
}