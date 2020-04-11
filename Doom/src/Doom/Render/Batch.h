#pragma once
#include "../pch.h"
#include "Renderer.h"
#include "Character.h"
#include "../Gui/GuiItem.h"
#include <array>
#include <mutex>

namespace Doom {

#define RENDERER_MAX_SPRITES   10000
#define RENDERER_VERTEX_SIZE   sizeof(Vertex)
#define RENDERER_SPRITE_SIZE   RENDERER_VERTEX_SIZE * 4
#define RENDERER_BUFFER_SIZE   RENDERER_SPRITE_SIZE * RENDERER_MAX_SPRITES
#define RENDERER_INDICES_SIZE  RENDERER_MAX_SPRITES * 6

	struct DOOM_API Vertex {
		glm::vec2 vertex;
		glm::vec2 textcoords;
		int m_static;
		glm::vec4 m_color;
		float isGui;
		float texIndex;
		int m_particle;
		glm::vec4 rotationMat0;
		glm::vec4 rotationMat1;
		glm::vec4 rotationMat2;
		glm::vec4 rotationMat3;
	};

	class DOOM_API Batch {
		bool isBegan = false;
		std::mutex mtx;

		Texture* whitetexture = new Texture();
		void initText();
		void initGameObjects();

		const unsigned int maxTextureSlots = 32;
		std::array<uint32_t, 32> textureSlots;
		unsigned int textureSlotsIndex = 1;
		float textureIndex = 0;


		Vertex* buffer;
		GLuint vao;
		GLuint vbo;
		IndexBuffer* ibo;


		std::array<uint32_t, 32> GtextureSlots;
		unsigned int GtextureSlotsIndex = 2;
		float GtextureIndex = 0;


		Vertex* Gbuffer;
		GLuint Gvao;
		GLuint Gvbo;
		IndexBuffer* Gibo;

		static Batch* instance;

		Shader* shader = nullptr;
		glm::mat4 ViewProjecTionRelatedToCamera = glm::ortho(-16.f, 16.f, -9.f, 9.f, 1.f, -1.f);
	public:
		static void Init();

		GLsizei indexcount = 0;

		GLsizei Gindexcount = 0;

		inline static Batch* GetInstance() { return instance; }
		Shader* TextShader = new Shader("src/Shaders/Text.shader");
		Shader* BasicShader = new Shader("src/Shaders/Basic.shader");
		Shader* CollisionShader = new Shader("src/Shaders/newshader.shader");
		Batch();
		~Batch();
		void Submit(Character& character);
		void Submit(GuiItem& gameobeject);
		void SubmitG(GameObject& gameobeject);
		void Submit(Collision& collision);
		void flushGameObjects(Shader* shader);
		void flushCollision(Shader* shader);
		void flushText(Shader* shader);
		bool isValidBuffer() { return (Gbuffer != nullptr && isBegan && Gbuffer != (void*)0x00000000); }

		void Begin();
		void End();
		void BeginGameObjects();
		void EndGameObjects();
	};

}