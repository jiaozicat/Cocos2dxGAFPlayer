#include "GAFPrecompiled.h"
#include "GAFAsset.h"
#include "GAFTextureAtlas.h"
#include "GAFTextureAtlasElement.h"
#include "GAFAnimatedObject.h"
#include "GAFAssetTextureManager.h"

#include "GAFLoader.h"

static float  _desiredCsf = 1.f;

float GAFAsset::desiredCsf()
{
    return cocos2d::CC_CONTENT_SCALE_FACTOR();
}

void GAFAsset::setDesiredCsf(float csf)
{
    _desiredCsf = csf;
}

GAFAnimatedObject * GAFAsset::createObject()
{
    if (m_timelines.empty())
    {
        return nullptr;
    }

    return GAFAnimatedObject::create(this, m_rootTimeline);
}

GAFAnimatedObject * GAFAsset::createObjectAndRun(bool looped)
{
    GAFAnimatedObject * res = createObject();
    if (res)
    {
        res->setLooped(looped);
        res->start();
    }
    return res;
}

GAFAsset::GAFAsset():
m_textureLoadDelegate(nullptr),
m_sceneFps(60),
m_sceneWidth(0),
m_sceneHeight(0),
m_rootTimeline(nullptr)
{
    m_textureManager = new GAFAssetTextureManager();
}

GAFAsset::~GAFAsset()
{
    GAF_RELEASE_MAP(Timelines_t, m_timelines);
    //CC_SAFE_RELEASE(m_rootTimeline);
    m_textureManager->release();
}

bool GAFAsset::isAssetVersionPlayable(const char * version)
{
    return true;
}

GAFAsset* GAFAsset::create(const std::string& gafFilePath, GAFTextureLoadDelegate* delegate)
{
    GAFAsset * ret = new GAFAsset();
    if (ret && ret->initWithGAFFile(gafFilePath, delegate))
    {
        ret->autorelease();
        return ret;
    }
    CC_SAFE_RELEASE(ret);
    return NULL;
}


GAFAsset* GAFAsset::createWithBundle(const std::string& zipfilePath, const std::string& entryFile, GAFTextureLoadDelegate* delegate /*= NULL*/)
{
    GAFAsset * ret = new GAFAsset();
    if (ret && ret->initWithGAFBundle(zipfilePath, entryFile, delegate))
    {
        ret->autorelease();
        return ret;
    }
    CC_SAFE_RELEASE(ret);
    return NULL;
}

bool GAFAsset::initWithGAFBundle(const std::string& zipFilePath, const std::string& entryFile, GAFTextureLoadDelegate* delegate /*= NULL*/)
{
    GAFLoader* loader = new GAFLoader();

    std::string fullfilePath = cocos2d::FileUtils::getInstance()->fullPathForFilename(zipFilePath);

    cocos2d::ZipFile bundle(fullfilePath);
    ssize_t sz = 0;
    unsigned char* gafData = bundle.getFileData(entryFile, &sz);

    if (!gafData || !sz)
        return false;

    bool isLoaded = loader->loadData(gafData, sz, this);

    if (isLoaded)
    {
        /*_chooseTextureAtlas();

        if (m_currentTextureAtlas)
        {
            m_textureLoadDelegate = delegate;
            m_currentTextureAtlas->loadImages(entryFile, m_textureLoadDelegate, &bundle);
        }
		*/
    }

    delete loader;

    return isLoaded;
}

bool GAFAsset::initWithGAFFile(const std::string& filePath, GAFTextureLoadDelegate* delegate)
{
    GAFLoader* loader = new GAFLoader();

    std::string fullfilePath = cocos2d::FileUtils::getInstance()->fullPathForFilename(filePath);

    bool isLoaded = loader->loadFile(fullfilePath, this);

	if (m_timelines.empty())
    {
        return false;
    }
    if (isLoaded)
    {
		for (Timelines_t::iterator i = m_timelines.begin(), e = m_timelines.end(); i != e; i++)
		{
			i->second->loadImages();

			if (i->second->getTextureAtlas())
			{
				m_textureManager->appendInfoFromTextureAtlas(i->second->getTextureAtlas());
				//i->second->getTextureAtlas()->loadImages(fullfilePath, m_textureLoadDelegate);
			}
		}

		m_textureLoadDelegate = delegate;
		m_textureManager->loadImages(fullfilePath, m_textureLoadDelegate);
    }

    delete loader;

    return isLoaded;
}

/*GAFTextureAtlas* GAFAsset::getTextureAtlas()
{
    return m_currentTextureAtlas;
}*/

void GAFAsset::setRootTimeline(GAFTimeline *tl)
{
    assert(!m_rootTimeline);
    m_rootTimeline = tl;
    //m_rootTimeline->retain();
}

GAFTimeline* GAFAsset::getRootTimeline() const
{
    return m_rootTimeline;
}

void GAFAsset::pushTimeline(uint32_t timelineIdRef, GAFTimeline* t)
{
	m_timelines[timelineIdRef] = t;
    t->retain();
}

void GAFAsset::setHeader(GAFHeader& h)
{
    m_header = h;
}

void GAFAsset::setTextureLoadDelegate(GAFTextureLoadDelegate* delegate)
{
    m_textureLoadDelegate = delegate;
}

GAFAssetTextureManager* GAFAsset::getTextureManager()
{
	return m_textureManager;
}

Timelines_t& GAFAsset::getTimelines()
{
    return m_timelines;
}

const Timelines_t& GAFAsset::getTimelines() const
{
	return m_timelines;
}

const GAFHeader& GAFAsset::getHeader() const
{
    return m_header;
}

const unsigned int GAFAsset::getSceneFps() const
{
    return m_sceneFps;
}

const unsigned int GAFAsset::getSceneWidth() const
{
    return m_sceneWidth;
}

const unsigned int GAFAsset::getSceneHeight() const
{
    return m_sceneHeight;
}

const cocos2d::Color4B& GAFAsset::getSceneColor() const
{
    return m_sceneColor;
}

void GAFAsset::setSceneFps(unsigned int value)
{
    m_sceneFps = value;
}

void GAFAsset::setSceneWidth(unsigned int value)
{
    m_sceneWidth = value;
}

void GAFAsset::setSceneHeight(unsigned int value)
{
    m_sceneHeight = value;
}

void GAFAsset::setSceneColor(const cocos2d::Color4B& value)
{
    m_sceneColor = value;
}
