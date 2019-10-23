#include "TextureLoader.h"
#include "Application.h"
#include "ModuleRenderer3D.h"

// Include DevIL headers
#include "libs/DevIL Windows SDK/include/IL/il.h"
#include "libs/DevIL Windows SDK/include/IL/ilu.h"
#include "libs/DevIL Windows SDK/include/IL/ilut.h"

// Include DevIL libs
#pragma comment (lib, "libs/DevIL Windows SDK/lib/x86/Release/DevIL.lib")
#pragma comment (lib, "libs/DevIL Windows SDK/lib/x86/Release/ILU.lib")
#pragma comment (lib, "libs/DevIL Windows SDK/lib/x86/Release/ILUT.lib")

// Default texture data
#define CHECKERS_HEIGHT 64
#define CHECKERS_WIDTH 64

TextureLoader::TextureLoader(Application* app, const char* name, bool start_enabled) : Module(app, name, start_enabled)
{}

bool TextureLoader::Init()
{
	bool ret = true;

	// Check versions
	if (ilGetInteger(IL_VERSION_NUM) < IL_VERSION ||
		iluGetInteger(ILU_VERSION_NUM) < ILU_VERSION ||
		ilutGetInteger(ILUT_VERSION_NUM) < ILUT_VERSION)
	{
		LOG("[Error]: DevIL version is too old and needs updating to work.");

		ret = false;
		SDL_assert(ret);
	}

	LOG("[Info]: Initializing DevIL");
	ilInit();
	iluInit();
	ilutInit();
	ilutRenderer(ILUT_OPENGL);	// Initialize ILUT with OpenGL support

	App->hardware.devil_info.vendor = (unsigned char*)ilGetString(IL_VENDOR);
	App->hardware.devil_info.version = (unsigned char*)ilGetString(IL_VERSION_NUM);

	return ret;
}

bool TextureLoader::Start()
{
	// --- Load Checkers Texture ---
	defaultTex = LoadDefaultTex();

	return true;
}

bool TextureLoader::CleanUp()
{
	LOG("[Info]: Deleting Textures.");
	if (defaultTex > 0)
		glDeleteTextures(1, (GLuint*)&defaultTex);

	LOG("[Info]: Shutting down DevIL.");
	ilShutDown();

	return true;
}

// ---------------------------------------------

uint TextureLoader::CreateTexture(const void* imgData, uint width, uint height, int internalFormat, uint format, uint target, int filterType, int fillingType, bool defaultTex) const
{
	GLuint texId = 0;
	std::string filterStr, texType = "Texture 2D";

	// Set pixel storage modes (https://www.khronos.org/registry/OpenGL-Refpages/es2.0/xhtml/glPixelStorei.xml)
	glPixelStorei(GL_UNPACK_ALIGNMENT, 1);

	// Generate texture name (https://www.khronos.org/registry/OpenGL-Refpages/es2.0/xhtml/glGenTextures.xml)
	glGenTextures(1, &texId);

	// Bind a named texture to a texturing target (https://www.khronos.org/registry/OpenGL-Refpages/es2.0/xhtml/glBindTexture.xml)
	glBindTexture(target, texId);

	// Set texture parameters (https://www.khronos.org/registry/OpenGL-Refpages/es2.0/xhtml/glTexParameter.xml)
	glTexParameteri(target, GL_TEXTURE_WRAP_S, fillingType);
	glTexParameteri(target, GL_TEXTURE_WRAP_T, fillingType);

	if (filterType == GL_LINEAR) {	// Linear filter method makes the image blurry when made bigger
		filterStr.assign("Linear");
		glTexParameteri(target, GL_TEXTURE_MAG_FILTER, GL_LINEAR);
		glTexParameteri(target, GL_TEXTURE_MIN_FILTER, GL_LINEAR_MIPMAP_LINEAR);

		// If anisotropic filtering is available, use it
		if (glewIsSupported("GL_EXT_texture_filter_anisotropic"))
		{
			filterStr.assign("Linear Anisotropic Filtering");

			GLfloat maxAnisotrpy;
			glGetFloatv(GL_MAX_TEXTURE_MAX_ANISOTROPY_EXT, &maxAnisotrpy);
			glTexParameterf(target, GL_TEXTURE_MAX_ANISOTROPY_EXT, maxAnisotrpy);
		}
	}
	else if (filterType == GL_NEAREST) {		// Nearest filter method makes the image pixelated when made bigger
		filterStr.assign("Nearest");
		if (!defaultTex)
		{
			glTexParameteri(target, GL_TEXTURE_MAG_FILTER, GL_NEAREST);
			glTexParameteri(target, GL_TEXTURE_MIN_FILTER, GL_NEAREST_MIPMAP_NEAREST/*GL_NEAREST*/);	//WARNING! Originally was GL_Nearest for the last parameter, revert in case of texture trouble
		} 
		else {
			glTexParameteri(target, GL_TEXTURE_MAG_FILTER, GL_NEAREST);
			glTexParameteri(target, GL_TEXTURE_MIN_FILTER, GL_NEAREST);
		}
	}
	else {
		LOG("[Error]: Texture loading was done with a filter type that wasn't GL_NEAREST nor GL_LINEAR.")
#ifdef _DEBUG
			SDL_TriggerBreakpoint();
#endif
	}

	// Specify a two-dimensional texture image (https://www.khronos.org/registry/OpenGL-Refpages/gl4/html/glTexImage2D.xhtml)
	glTexImage2D(target, 0, internalFormat, width, height, 0, format, GL_UNSIGNED_BYTE, imgData);

	if (!defaultTex)	// If texture isn't the default one
	{
		if (target == GL_TEXTURE_2D) {	// Change GL_Setting related to texture mode depending on the texture loaded
			if (!App->renderer3D->GL_Texture2D.status) {
				App->renderer3D->SwitchGroupGLSetting(App->renderer3D->GL_Texture2D, App->renderer3D->GL_Texture2D.group);
				App->editor->textureMode = (int)texture_mode::TWO_D;
			}
		}
		else if (target == GL_TEXTURE_CUBE_MAP) {
			texType.assign("Cube Map");
			if (!App->renderer3D->GL_TextureCubeMap.status) {
				App->renderer3D->SwitchGroupGLSetting(App->renderer3D->GL_TextureCubeMap, App->renderer3D->GL_TextureCubeMap.group);
				App->editor->textureMode = (int)texture_mode::CUBEMAP;
			}
		}
		else {
			LOG("[Error]: Texture loading was done with a target type that wasn't GL_TEXTURE_2D nor GL_TEXTURE_CUBE_MAP.")
#ifdef _DEBUG
				SDL_TriggerBreakpoint();
#endif
		}
		
		glGenerateMipmap(target);
	}

	// Unbind Texture
	glBindTexture(target, 0);

	LOG("[Success]: Loaded Texture: ID: %i , Width: %i , Height: %i, Filter: %s, Texture Type: %s", texId, width, height, filterStr.c_str(), texType.c_str());	//CHANGE/FIX: This pops up even if an error happened on the way, fix

	return texId;
}

uint TextureLoader::LoadTextureFile(const char* path, uint target, int filterType, int fillingType) const
{
	if (path == nullptr)
	{
		LOG("[Error]: Texture path attempted to load was NULL.");
		SDL_assert(path != nullptr);
		return 0;
	}

	uint imgId = 0;
	uint texId = 0;

	ilGenImages(1, (ILuint*)&imgId);	// Generate image inside DevIL
	ilBindImage(imgId);					// Bind our image Id to DevIL's

	// Load Image //IMPROVE: iLoad(type, fileName) skips checking the type if it is specified (Check Manual Appendix B)
	if (ilLoadImage(path))
	{
		// Get Image Data
		ILinfo imgInfo;
		iluGetImageInfo(&imgInfo);
		
		// If flipped, flip it back
		if (imgInfo.Origin == IL_ORIGIN_UPPER_LEFT)
			if (!iluFlipImage()) {
				LOG("[Error]: Couldn't flip image. Cause: %s", iluErrorString(ilGetError()));
				SDL_assert(false);
			}

		// Transform the Image into data OpenGL can use //IMPROVE: Should we always assume that the incoming image is in RGBA?
		if (ilConvertImage(IL_RGBA, IL_UNSIGNED_BYTE))
		{
			// Create texture and assign ID
			texId = CreateTexture(ilGetData(), ilGetInteger(IL_IMAGE_WIDTH), ilGetInteger(IL_IMAGE_HEIGHT), ilGetInteger(IL_IMAGE_FORMAT), ilGetInteger(IL_IMAGE_FORMAT), target, filterType, fillingType);
			LOG("[Success]: Loaded texture from path %s", path);	//IMPROVE: Add error call here related to texId?
		}
		else {
			LOG("[Error]: Image conversion failed. Cause: %s", iluErrorString(ilGetError()));
			SDL_assert(false);
		}
	}
	else {
		LOG("[Error]: Image loading failed. Cause: %s", iluErrorString(ilGetError()));
		SDL_assert(false);
	}

	ilDeleteImages(1, (const ILuint*)&imgId);	// Delete image inside DevIL

	return texId;
}

// ---------------------------------------------

uint TextureLoader::LoadDefaultTex() const
{
	GLubyte checkImage[CHECKERS_HEIGHT][CHECKERS_WIDTH][4];
	for (int i = 0; i < CHECKERS_HEIGHT; i++) {
		for (int j = 0; j < CHECKERS_WIDTH; j++) {
			int c = ((((i & 0x8) == 0) ^ (((j & 0x8)) == 0))) * 255;
			checkImage[i][j][0] = (GLubyte)c;
			checkImage[i][j][1] = (GLubyte)c;
			checkImage[i][j][2] = (GLubyte)c;
			checkImage[i][j][3] = (GLubyte)255;
		}
	}

	return CreateTexture(checkImage, CHECKERS_WIDTH, CHECKERS_HEIGHT, GL_RGBA, GL_RGBA, GL_TEXTURE_2D, GL_NEAREST, GL_REPEAT, true);
}

uint TextureLoader::GetDefaultTex() const
{
	return defaultTex;
}