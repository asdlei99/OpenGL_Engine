#ifndef RENDERER_H
#define RENDERER_H

#include "../Render/Renderer2D.h"
#include "../Render/RenderText.h"
#include "../Components/Collision.h"
#include "../Core/Editor.h"
#include "../ECS/ComponentManager.h"
#include <mutex>

namespace Doom {

	class DOOM_API Renderer {
	public:
		static int DrawCalls;
		static bool isReadyToRenderFirstThread;
		static bool isReadyToRenderSecondThread;
		static bool isReadyToRenderThirdThread;
		static void DeleteObject(int id);
		static void DeleteAll();
		static void SubmitGameObjects(OrthographicCamera& camera);
		static void Render();
		static void CalculateMVPforAllObjects();
		static void RenderText();
		static void RenderCollision(OrthographicCamera& camera);
		static void Clear();
		static void Save(const std::string filename);
		static void Load(const std::string filename);
		static std::vector<unsigned int> CalculateObjectsVectors();
		static std::vector<unsigned int> GetObjectsWithNoOwnerReference() { return ObjectsWithNoOwner; }
		static std::vector<unsigned int> GetObjectsWithOwnerReference() { return ObjectsWithOwner; }
		inline static unsigned int GetAmountOfObjects() { return Renderer2DLayer::objects2d.size(); }
		inline static unsigned int GetAmountOfCollisions() { return Renderer2DLayer::collision2d.size(); }
		static Renderer2DLayer* GetReference(int id) { return &Renderer2DLayer::objects2d[id].get(); }
		static const char** items;
		static const char** GetItems() {
			delete[] items;
			items = new const char*[GetObjectsWithNoOwnerReference().size()];
			for (unsigned int i = 0; i < GetObjectsWithNoOwnerReference().size(); i++)
			{
				int id = GetObjectsWithNoOwnerReference()[i];
					items[i] = Renderer2DLayer::objects2d[id].get().name->c_str();
				
			}
			return items;
		}

		static GameObject* CreateGameObject() {
			GameObject * go = new GameObject("Unnamed", 0, 0);
			return go;
		}
	private:
		static std::vector<unsigned int> ObjectsWithNoOwner;
		static std::vector<unsigned int> ObjectsWithOwner;
		static std::mutex mtx;
		static std::condition_variable condVar;
		Renderer() {}

		template<typename T>
		static void LoadObj(std::string name, std::string pathtotext, float angle, float color[4],
			float scale[3], double pos[2], int shadertype, bool hascollision, float* offset, int rendertype, int* axes, bool istrigger, std::string tag,float* uvs,bool isSprite,float* spriteSize) {
			T* go = new T(name, pos[0], pos[1]);
			go->GetComponentManager()->GetComponent<Transform>()->Scale(scale[0], scale[1]);
			go->GetComponentManager()->GetComponent<Transform>()->RotateOnce(angle, glm::vec3(axes[0], axes[1], axes[2]));
			go->SetRenderType((Renderer2DLayer::RenderType)rendertype);
			go->mesh2D[2] = uvs[0];
			go->mesh2D[3] = uvs[1];
			go->mesh2D[6] = uvs[2];
			go->mesh2D[7] = uvs[3];
			go->mesh2D[10] = uvs[4];
			go->mesh2D[11] = uvs[5];
			go->mesh2D[14] = uvs[6];
			go->mesh2D[15] = uvs[7];
			go->SetColor(glm::vec4(color[0], color[1], color[2], color[3]));
			if(pathtotext != "None")
				go->SetTexture(pathtotext);
			if (isSprite) {
				for (unsigned int i = 0; i < TextureAtlas::textureAtlases.size(); i++)
				{
					if (TextureAtlas::textureAtlases[i]->m_texture->GetFilePath() == pathtotext) {
						go->textureAtlas = TextureAtlas::textureAtlases[i];
					}
				}
				if(go->textureAtlas == nullptr)
					go->textureAtlas = new TextureAtlas(spriteSize[0], spriteSize[1], pathtotext);
			}
			if (hascollision == 1) {
				go->GetComponentManager()->AddComponent<Collision>();
				go->GetComponentManager()->GetComponent<Collision>()->Translate(pos[0], pos[1]);
				go->GetComponentManager()->GetComponent<Collision>()->Enable = true;
				go->GetComponentManager()->GetComponent<Collision>()->SetOffset(offset[0], offset[1]);
				go->GetComponentManager()->GetComponent<Collision>()->IsTrigger = istrigger;
				go->GetComponentManager()->GetComponent<Collision>()->SetTag(tag);
				go->GetComponentManager()->GetComponent<Transform>()->Scale(scale[0], scale[1]);
			}
		}

	};

}

#endif