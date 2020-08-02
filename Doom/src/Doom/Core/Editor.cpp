#include "../pch.h"
#include "Editor.h"
#include <iostream>
#include <filesystem>
#include "ColoredOutput.h"
#include "../Render/Gui.h"
namespace fs = std::filesystem;

using namespace Doom;

void Editor::EditorUpdate()
{
	Debug();

	Threads();

	ImGui::Begin("Console");

	if (ImGui::Button("Save")) {
		Renderer::Save("C:/Users/Alexandr/Desktop/saved.txt");
	}
	ImGui::SameLine();
	if (ImGui::Button("Load")) {
		Renderer::Load("C:/Users/Alexandr/Desktop/saved.txt");
	}
	ImGui::SameLine();
	if (ImGui::Button("EXIT")) {
		glfwSetWindowShouldClose(Window::GetWindow(), GLFW_TRUE);
	}
	if (ImGui::Button("Create Atlas")) {
		IsActiveTextureAtlasCreation = !IsActiveTextureAtlasCreation;
	}
	CreateTextureAtlas();

	ImGui::SliderFloat("Zoom", &Window::GetCamera().zoomlevel, 0.1f, 100.f);
	Window::GetCamera().Zoom(abs(Window::GetCamera().GetZoomLevel()));
	ImGui::End();

	ImGui::Begin("Scene");
	if (ImGui::BeginPopupContextWindow())
	{
		if (ImGui::MenuItem("Create GameObject"))
		{
			Renderer::CreateGameObject();
			Renderer::CalculateObjectsVectors().size();
			go = Renderer::objects2d.back();
		}
		if (ImGui::MenuItem("Clone"))
		{
			GameObject* clgo = Renderer::CreateGameObject();
			clgo->operator=(*go);
			go = clgo;
		}
			
		if (ImGui::MenuItem("Delete"))
		{
			if (go != nullptr) {
				Renderer::DeleteObject(go->GetId());
				if(Renderer::objects2d.size() > 0)
					go = Renderer::objects2d[Renderer::objects2d.size() - 1];
				else
					go = nullptr;
			}
			ImGui::EndPopup();
			ImGui::End();
			return;
		}
		ImGui::EndPopup();
	}

	ImGui::SetWindowFontScale(1.25);
	Renderer::CalculateObjectsVectors();

	
	if (Renderer::objects2d.size() > 0 && go == nullptr) {
		go = Renderer::objects2d[0];
	}

	if (go != nullptr) {
		gizmo->obj = go;
	}

	if (ImGui::CollapsingHeader("Game Objects")) {
		unsigned int amount = Renderer::GetAmountOfObjects();
		for (unsigned int i = 0; i < amount; i++)
		{
			GameObject* go = Renderer::objects2d[i];
			if (go->GetOwner() != nullptr) {
				continue;
			}
			ImGui::PushID(go->GetId());
			if (this->go == go && ImGui::IsItemVisible()) {
				ImVec2 sp = ImGui::GetCursorScreenPos();
				ImGui::GetForegroundDrawList()->AddRectFilled(ImVec2(sp.x, sp.y), ImVec2(ImGui::GetWindowWidth() - 20, sp.y + ImGui::GetItemRectSize().y), IM_COL32(80, 80, 80, 100));
			}
			if (ImGui::TreeNode(go->name.c_str())) {
				if (ImGui::IsItemDeactivated() || ImGui::IsItemActivated())
					this->go = go;
				unsigned int childsAmount = go->GetChilds().size();
				if (childsAmount > 0) {
					for (unsigned int j = 0; j < childsAmount; j++)
					{
						GameObject* child = static_cast<GameObject*>(go->GetChilds()[j]);
						ImGui::Indent();
						ImGui::PushID(child->GetId());
						if (this->go == child && ImGui::IsItemVisible()) {
							ImVec2 sp = ImGui::GetCursorScreenPos();
							ImGui::GetForegroundDrawList()->AddRectFilled(ImVec2(sp.x, sp.y), ImVec2(ImGui::GetWindowWidth() - 20, sp.y + ImGui::GetItemRectSize().y), IM_COL32(80, 80, 80, 100));
						}
						if (ImGui::TreeNode(child->name.c_str())) {
							if (ImGui::IsItemDeactivated() || ImGui::IsItemActivated())
								this->go = child;
							unsigned int childsAmount = child->GetChilds().size();
							if (childsAmount > 0) {
								for (unsigned int j = 0; j < childsAmount; j++)
								{
									GameObject* child1 = static_cast<GameObject*>(child->GetChilds()[j]);
									ImGui::PushID(child1->GetId());
									ImGui::Indent();
									if (this->go == child1 && ImGui::IsItemVisible()) {
										ImVec2 sp = ImGui::GetCursorScreenPos();
										ImGui::GetForegroundDrawList()->AddRectFilled(ImVec2(sp.x, sp.y), ImVec2(ImGui::GetWindowWidth() - 20, sp.y + ImGui::GetItemRectSize().y), IM_COL32(80, 80, 80, 100));
									}
									if (ImGui::TreeNode(child1->name.c_str())) {
										if (ImGui::IsItemDeactivated() || ImGui::IsItemActivated())
											this->go = child1;
										ImGui::TreePop();
									}
									ImGui::PopID();
									ImGui::Unindent();
								}
							}
							ImGui::TreePop();
						}
						ImGui::PopID();
						ImGui::Unindent();
					}
				}
				ImGui::TreePop();
			}
			ImGui::PopID();
		}
	}

	ImGui::NewLine();
	ImGui::End();

	ImGui::Begin("Properties");
	 {
		ImGui::SetWindowFontScale(1.25);
		if (Renderer::GetObjectsWithNoOwnerReference().size() > 0) {
			if (go == nullptr)
				return;
			Doom::Transform* tr = go->component_manager->GetComponent<Doom::Transform>();
			Doom::RectangleCollider2D* col = go->component_manager->GetComponent<Doom::RectangleCollider2D>();
			ImGui::Text("ID %d", go->GetId());
			ImGui::Checkbox("Enable", &go->Enable);
			ImGui::Checkbox("Static", &go->Static);
			if (go->GetOwner() != nullptr)
				ImGui::Text("Has Owner");
			ImGui::InputText("Name", name, sizeof(name));
			ImGui::SameLine();
			if (ImGui::Button("Change name")) {
				go->SetName(name);
			}
			ImGui::SliderInt("Layer", &go->GetLayer(), 0, Renderer::GetAmountOfObjects() - 1);
			if (go->GetComponentManager()->GetComponent<Irenderer>()->renderType == "2D") {
				if (ImGui::Button("Change layer")) {
					if (go->GetLayer() > Renderer::GetAmountOfObjects() - 1) {
						std::cout << "Error: layer out of range" << std::endl;
						return;
					}
					go->GetComponent<SpriteRenderer>()->Setlayer(go->GetLayer());
					Renderer::CalculateObjectsVectors();
				}
			}

			TransformComponent(tr);
			Renderer2DComponent();
			Animator2DComponent();
			CubeCollider3DComponent();
			RectangleCollider2D(col);
			LightPointComponent();
			DirPointComponent();
			MaterialComponent();
			ImGui::NewLine();
			ImGui::Indent(ImGui::GetWindowSize().x * 0.4);
			if (col == nullptr) {
				if (ImGui::Button("Add collision")) {
					go->GetComponentManager()->AddComponent<Doom::RectangleCollider2D>();
				}
			}
			
			ImGui::NewLine();
			if (ImGui::Button("Add Child")) {
				GameObject* child1 = new GameObject("Child", 0, 0);
				go->AddChild((void*)child1);
				child1->SetOwner((void*)go);
			}
			ImGui::Unindent();

		}
		
	}

	ImGui::End();
}

void Editor::CreateTextureAtlas() {
	if (!IsActiveTextureAtlasCreation)
		return;
	ImGui::Begin("Texture Atlas creation", &IsActiveTextureAtlasCreation);
	ImGui::InputFloat2("Sprite size", spriteSize);
	ImGui::InputText("Path to texture folder from solution directory", pathToTextureFolder, 64);
	if(ImGui::Button("Check Folder")) {
		std::string path = pathToTextureFolder;
		CheckTexturesFolder(path);
	}

	int counterImagesButtons = 0;
	ImGui::Text("Textures");
	for (auto i = Texture::textures.begin(); i != Texture::textures.end();i++)
	{
		void* my_tex_id = reinterpret_cast<void*>(i->second->m_RendererID);
		int frame_padding = -1;
		if (counterImagesButtons > 6) {
			ImGui::NewLine();
			counterImagesButtons = 0;
		}

		if (ImGui::ImageButton(my_tex_id, ImVec2(36, 36), ImVec2(1, 1), ImVec2(0, 0), frame_padding, ImVec4(1.0f, 1.0f, 1.0f, 0.5f))) {
			textureForAtlas = i->second;
		}

		ImGui::SameLine();
		counterImagesButtons++;
	}

	ImGui::NewLine();
	if (ImGui::Button("Apply") &&  textureForAtlas != nullptr) {
		IsActiveTextureAtlasCreation = false;
		TextureAtlas* textureAtlas = new TextureAtlas(spriteSize[0],spriteSize[1],textureForAtlas);
	}
	ImGui::End();
}

void Doom::Editor::MaterialComponent()
{
	if (go->GetComponentManager()->GetComponent<Irenderer>()->renderType == "3D") {
		if (ImGui::CollapsingHeader("Renderer 3D")) {
			Renderer3D* r = static_cast<Renderer3D*>(go->GetComponentManager()->GetComponent<Irenderer>());
			ImGui::Indent(ImGui::GetWindowSize().x * 0.05);
			if (ImGui::CollapsingHeader("Material")) {
				ImGui::SliderFloat("Ambient", &r->mat.ambient, 0, 1);
				ImGui::SliderFloat("Specular", &r->mat.specular, 0, 50);
				ImGui::Checkbox("NormalMap", &r->useNormalMap);
				float* tempColor = r->GetColor();
				ImGui::ColorEdit4("Color", tempColor);
				r->SetColor(glm::vec4(tempColor[0], tempColor[1], tempColor[2], tempColor[3]));
			}
			if (ImGui::CollapsingHeader("Mesh")) {
				ImGui::Text("%s", r->mesh->name);
			}
			ImGui::Unindent();
		}
	}
}

void Doom::Editor::CubeCollider3DComponent()
{
	if (go->GetComponentManager()->GetComponent<CubeCollider3D>() != nullptr) {
		CubeCollider3D* c = go->GetComponentManager()->GetComponent<CubeCollider3D>();
		if (ImGui::CollapsingHeader("CubeCollider3D")) {
			ImGui::SliderFloat3("Position", &c->offset.x, -50, 50);
			ImGui::SliderFloat3("Scale", &c->scale.x, 0, 100);
		}
	}
}

void Doom::Editor::Renderer2DComponent()
{
	if (go->GetComponentManager()->GetComponent<Irenderer>() != nullptr && go->GetComponentManager()->GetComponent<Irenderer>()->renderType == "2D" && ImGui::CollapsingHeader("Render2D")) {
		SpriteRenderer* sr = go->GetComponent<SpriteRenderer>();
		color = sr->GetColor();
		ImGui::ColorEdit4("Color", color);
		sr->SetColor(glm::vec4(color[0], color[1], color[2], color[3]));
		delete[] color;
		int counterImagesButtons = 0;
		ImGui::Text("Textures");
		for (unsigned int i = 0; i < texture.size(); i++)
		{
			void* my_tex_id = reinterpret_cast<void*>(texture[i]->m_RendererID);
			int frame_padding = -1;
			if (counterImagesButtons > 6) {
				ImGui::NewLine();
				counterImagesButtons = 0;
			}

			if (ImGui::ImageButton(my_tex_id, ImVec2(36, 36), ImVec2(1, 1), ImVec2(0, 0), frame_padding, ImVec4(1.0f, 1.0f, 1.0f, 0.5f))) {
				if (go != nullptr) {

					sr->SetTexture(texture[i]);
				}

			}

			ImGui::SameLine();
			counterImagesButtons++;
		}

		ImGui::NewLine();
		ImGui::InputFloat2("UVs Offset", uvsOffset);

		if (ImGui::Button("Use these UVs")) {
			if (go != nullptr && sr->textureAtlas != nullptr)
				sr->SetUVs(sr->textureAtlas->GetSpriteUVs(uvsOffset[0], uvsOffset[1]));
		}
		if (ImGui::Button("Original UVs")) {
			if (go != nullptr)
				sr->OriginalUvs();
		}
		if (ImGui::Button("No texture")) {
			if (go != nullptr)
				sr->SetTexture(nullptr);
		}
		ImGui::InputText("path", pathToTextureFolder, 64);
		if (ImGui::Button("Refresh textures")) {
			texturesPath.clear();
			texture.clear();
			CheckTexturesFolderUnique(pathToTextureFolder);
		}
		int prevselectedAtlas = selectedAtlas;
		if (TextureAtlas::textureAtlases.size() > 0) {
			ImGui::ListBox("Texture atlases", &selectedAtlas, TextureAtlas::GetTextureAtlases(), TextureAtlas::textureAtlases.size());
		}
		if (selectedAtlas != -1) {
			if (selectedAtlas != prevselectedAtlas)
				tool_active = true;
			if (tool_active) {
				ImGui::Begin("Texture Atlas", &tool_active);

				Texture* textureOfAtlas = TextureAtlas::textureAtlases[selectedAtlas]->GetTexture();
				int frame_padding = -1;
				unsigned int amountOfSpritesX = (textureOfAtlas->GetWidth()) / (TextureAtlas::textureAtlases[selectedAtlas]->GetSpriteWidth());
				unsigned int amountOfSpritesY = (textureOfAtlas->GetHeight()) / (TextureAtlas::textureAtlases[selectedAtlas]->GetSpriteHeight());
				for (unsigned int i = 0; i < amountOfSpritesY; i++)
				{
					for (unsigned int j = 0; j < amountOfSpritesX; j++)
					{
						float* uvs = TextureAtlas::textureAtlases[selectedAtlas]->GetSpriteUVs(j, amountOfSpritesY - i);
						ImGui::PushID((i * amountOfSpritesX) + j);
						if (ImGui::ImageButton((void*)(intptr_t)textureOfAtlas->m_RendererID, ImVec2(56, 56), ImVec2(uvs[0], uvs[5]), ImVec2(uvs[4], uvs[1]), frame_padding, ImVec4(1.0f, 1.0f, 1.0f, 0.5f)))
						{
							if (go != nullptr) {
								SpriteRenderer* sr = go->GetComponentManager()->GetComponent<SpriteRenderer>();
								sr->textureAtlas = TextureAtlas::textureAtlases[selectedAtlas];
								sr->SetTexture(textureOfAtlas);
								sr->SetUVs(uvs);
							}
						}
						ImGui::PopID();
						ImGui::SameLine();
					}
					ImGui::NewLine();
				}
				ImGui::NewLine();
				ImGui::InputFloat2("Sprite size", spriteSize);
				if (ImGui::Button("Apply")) {
					TextureAtlas::textureAtlases[selectedAtlas]->SetSpriteSize(spriteSize[0], spriteSize[1]);
				}
				ImGui::End();
			}
			else
				selectedAtlas = -1;
		}

	}
}

void Doom::Editor::RectangleCollider2D(Doom::RectangleCollider2D* col)
{
	if (col != nullptr) {
		if (ImGui::CollapsingHeader("Rectangle collider2D")) {
			ImGui::Text("Collision");
			ImGui::Text("ID %d", col->GetId());
			ImGui::Text("Tag %s", col->GetTag().c_str());
			ImGui::InputText("Tag", tag, sizeof(tag));
			ImGui::SameLine();
			if (ImGui::Button("Change tag")) {
				col->SetTag(tag);
			}
			ImGui::Checkbox("Enable collision", &col->Enable);
			ImGui::Checkbox("Trigger", &col->IsTrigger);
			ImGui::InputFloat2("Set the borders of X and Y offset slider", changeSliderCollisionOffset);
			ImGui::SliderFloat("Offset X", &col->offset.x, changeSliderCollisionOffset[0], changeSliderCollisionOffset[1]);
			ImGui::SliderFloat("Offset Y", &col->offset.y, changeSliderCollisionOffset[0], changeSliderCollisionOffset[1]);
			ImGui::InputFloat2("Offset", &(col->offset.x, col->offset.x));
			col->SetOffset(col->offset.x, col->offset.y);
			if (ImGui::Button("Remove collider")) {
				go->component_manager->RemoveComponent<Doom::RectangleCollider2D>();
				selectedcomponent = 0;
			}
		}
	}
}

void Doom::Editor::Animator2DComponent()
{
	if (go->GetComponentManager()->GetComponent<Animator>() != nullptr) {
		if (ImGui::CollapsingHeader("Animator")) {
			Animator* anim = go->GetComponentManager()->GetComponent<Animator>();
			ImGui::Text("Animator");
			ImGui::Text("counter %d", anim->counter);
			ImGui::ListBox("Animations", &selectedanimation, anim->GetAnimations(), anim->GetAmountOfAnimations());

			int count = 0;
			ImGui::SliderFloat("Animation speed slider", &anim->speed, 0, 100);
			if (anim->GetAmountOfAnimations() > 0) {
				auto iter = anim->animations.find(anim->GetAnimations()[selectedanimation]);
				if (iter._Ptr == nullptr)
					return;
				for (unsigned int i = 0; i < iter->second.size(); i++) {
					void* my_tex_id = reinterpret_cast<void*>((*iter).second[i]->m_RendererID);
					int frame_padding = -1;
					ImGui::Image(my_tex_id, ImVec2(42, 64), ImVec2(1, 1), ImVec2(0, 0), ImVec4(1.0f, 1.0f, 1.0f, 1.0f));
					if (count < 5) {
						ImGui::SameLine();
						count++;
					}
					else
						count = 0;
				}
				anim->selectedanim = selectedanimation;
			}
			ImGui::NewLine();
			ImGui::Checkbox("Play animation", &anim->isPlayingAnim);
		}
	}
}

void Doom::Editor::TransformComponent(Transform* tr)
{
	if (ImGui::CollapsingHeader("Transform")) {
		ImGui::Text("Position");
		ImGui::InputFloat2("Limits", changeSliderPos);
		ImGui::SliderFloat("Position X", &(tr->position.x), changeSliderPos[0], changeSliderPos[1]);
		ImGui::SliderFloat("Position Y", &(tr->position.y), changeSliderPos[0], changeSliderPos[1]);
		ImGui::SliderFloat("Position Z", &(tr->position.z), changeSliderPos[0], changeSliderPos[1]);
		ImGui::Text("Scale");
		ImGui::InputFloat2("Limits", changeSliderScale);
		ImGui::SliderFloat("Scale X", &(go->scaleValues[0]), changeSliderScale[0], changeSliderScale[1]);
		ImGui::SliderFloat("Scale Y", &(go->scaleValues[1]), changeSliderScale[0], changeSliderScale[1]);
		ImGui::SliderFloat("Scale Z", &(go->scaleValues[2]), changeSliderScale[0], changeSliderScale[1]);
		ImGui::Text("Rotate");
		ImGui::SliderAngle("Pitch", &tr->rotation.x);
		ImGui::SliderAngle("Yaw", &tr->rotation.y);
		ImGui::SliderAngle("Roll", &tr->rotation.z);
		tr->Scale(go->scaleValues[0], go->scaleValues[1], go->scaleValues[2]);
		tr->Translate(tr->position.x, tr->position.y, tr->position.z);
		tr->RotateOnce(tr->rotation.x, tr->rotation.y, tr->rotation.z, true);
	}
}

void Doom::Editor::LightPointComponent()
{
	PointLight* pl = go->GetComponentManager()->GetComponent<PointLight>();
	if (pl == nullptr)
		return;
	if (ImGui::CollapsingHeader("Point light")) {
		ImGui::SliderFloat("Constant",&pl->constant,0,1);
		ImGui::SliderFloat("Linear", &pl->linear,0,0.100f);
		ImGui::SliderFloat("Quadratic", &pl->quadratic,0,0.100);
		ImGui::ColorPicker3("Color", &pl->color[0]);
	}
}

void Doom::Editor::DirPointComponent()
{
	DirectionalLight* pl = go->GetComponentManager()->GetComponent<DirectionalLight>();
	if (pl == nullptr)
		return;
	if (ImGui::CollapsingHeader("Directional light")) {
		ImGui::ColorPicker3("Color", &pl->color[0]);
	}
}

void Doom::Editor::CheckTexturesFolderUnique(const std::string path)
{
	auto f = std::bind([](std::string path) {
		try
		{
			for (const auto & entry : fs::directory_iterator(path)) {
				std::string pathToTexture = entry.path().string();
				if (pathToTexture.find(".png") <= pathToTexture.length() || pathToTexture.find(".jpeg") <= pathToTexture.length()) {
					texturesPath.push_back(pathToTexture);
					size_t index = 0;
					index = texturesPath.back().find("\\", index);
					texturesPath.back().replace(index, 1, "/");
					Texture::LoadTextureInRAM(texturesPath.back(), true);
					texture.push_back(Texture::Get(texturesPath.back()));
				}
			}
		}
		catch (const std::exception&)
		{
				
		}
		
		for (unsigned int i = 0; i < texture.size(); i++)
		{
			std::function<void()> f2 = [=] {Texture::LoadTextureInVRAM(texture[i]->GetFilePath()); };
			std::function<void()>* f1 = new std::function<void()>(f2);
			EventSystem::GetInstance()->SendEvent("OnMainThreadProcess",nullptr,f1);
		}
		
		
	}, path);
	ThreadPool::Instance()->enqueue(f);
}

void Doom::Editor::CheckTexturesFolder(const std::string path)
{
	auto f = std::bind([](std::string path) {
		try
		{
			for (const auto & entry : fs::directory_iterator(path)) {
				std::string pathToTexture = entry.path().string();
				if (pathToTexture.find(".png") <= pathToTexture.length() || pathToTexture.find(".jpeg") <= pathToTexture.length()) {
					size_t index = 0;
					index = pathToTexture.find("\\", index);
					pathToTexture.replace(index, 1, "/");
					Texture::LoadTextureInRAM(pathToTexture, true);
					Texture* text = Texture::Get(pathToTexture);
					textureVecTemp.push_back(text);
				}
			}
		}
		catch (const std::exception&)
		{

		}

		for (unsigned int i = 0; i < textureVecTemp.size(); i++)
		{
			std::function<void()> f2 = [=] {Texture::LoadTextureInVRAM(textureVecTemp[i]->GetFilePath()); };
			std::function<void()>* f1 = new std::function<void()>(f2);
			EventSystem::GetInstance()->SendEvent("OnMainThreadProcess", nullptr, f1);
		}
		textureVecTemp.clear();

	}, path);
	ThreadPool::Instance()->enqueue(f);
}

void Doom::Editor::Debug()
{
	ImGui::Begin("Camera");
	ImGui::SliderAngle("X", &Window::GetCamera().pitch);
	ImGui::SliderAngle("Y", &Window::GetCamera().yaw);
	ImGui::SliderAngle("Z", &Window::GetCamera().roll);
	if (Window::GetCamera().type == Window::GetCamera().PERSPECTIVE) {
		ImGui::SliderAngle("fov", &Window::GetCamera().fovy, 60, 180);
		Window::GetCamera().SetFov(Window::GetCamera().fovy);
	}
	Window::GetCamera().SetRotation(glm::vec3(Window::GetCamera().pitch, Window::GetCamera().yaw, Window::GetCamera().roll));
	ImGui::End();
	ImGui::Begin("Debug");
	ImGui::Text("Application average %.3f ms/frame (%.1f FPS)", 1000.0f / ImGui::GetIO().Framerate, ImGui::GetIO().Framerate);
	ImGui::Text("Objects amount %d", Renderer::objects2d.size());
	ImGui::Text("Draw calls %d", Renderer::DrawCalls);
	ImGui::Text("Vertices %d", Renderer::Vertices);
	ImGui::Text("VRAM used %lf",Texture::VRAMused);
	ImGui::Text("Textures binded %d", Texture::bindedAmount);
	ImGui::Text("Textures amount %d", Texture::textures.size());
	ImGui::Checkbox("Polygon mode",&Renderer::PolygonMode);
	ImGui::Checkbox("Draw normals", &drawNormals);
	ImGui::Checkbox("Visible collisions", &RectangleCollider2D::IsVisible);
	ImGui::End();
	TextProps();
}

void Doom::Editor::Threads() {
	ImGui::Begin("Threads");
	for (auto i = ThreadPool::Instance()->isThreadBusy.begin(); i != ThreadPool::Instance()->isThreadBusy.end(); i++)
	{
		ImGui::Text("ID: %d task: %d",i->first,i->second);
	}
	ImGui::End();
}

void Doom::Editor::TextProps()
{
	Gui* g = Gui::GetInstance();
	ImGui::Begin("Text properties");
	ImGui::SliderFloat("Width", &g->textProps.width,0,0.9);
	ImGui::SliderFloat("Edge", &g->textProps.edge, 0.01, 0.9);
	ImGui::SliderFloat("Border width", &g->textProps.borderwidth, 0, 0.9);
	ImGui::SliderFloat("Border edge", &g->textProps.borderedge, 0.01, 0.9);
	ImGui::ColorPicker4("Out line color", TextColor);
	g->textProps.outLineColor = glm::vec4(TextColor[0], TextColor[1], TextColor[2], TextColor[3]);
	ImGui::SliderFloat2("Shaddow offset", shadowOffset,-0.01,0.01);
	g->textProps.shadowOffset = glm::vec2(shadowOffset[0], shadowOffset[1]);
	ImGui::End();
}

void Doom::Editor::UpdateNormals()
{
	std::lock_guard<std::mutex> lock(mtx_updateNormals);
	if (drawNormals) {
		uint32_t counter = 0;
		size_t size = Renderer::objects2d.size();
		for (size_t i = 0; i < size; i++)
		{
			Irenderer* iR = Renderer::objects2d[i]->GetComponent<Irenderer>();
			if (iR->renderType == "3D") {
				Renderer3D* r = static_cast<Renderer3D*>(iR);
				Mesh* mesh = r->mesh;
				size_t meshSize = r->mesh->meshSize;
				for (size_t j = 0; j < meshSize; j += 14)
				{
					glm::vec3 pos = glm::vec3(mesh->mesh[j + 0], mesh->mesh[j + 1], mesh->mesh[j + 2]);
					glm::vec3 normals = glm::vec3(mesh->mesh[j + 3], mesh->mesh[j + 4], mesh->mesh[j + 5]);
					glm::mat4 scaleXview = r->scale * r->view;
					glm::vec4 transformedPos = scaleXview * glm::vec4(pos.x, pos.y, pos.z, 0);
					glm::vec4 transformedNor = scaleXview * glm::vec4(normals.x, normals.y, normals.z, 0);
					this->normals[counter]->SetStartPoint(Renderer::objects2d[i]->position + (glm::vec3)transformedPos);
					this->normals[counter]->SetEndPoint(Renderer::objects2d[i]->position + (glm::vec3)transformedPos + glm::vec3(transformedNor));
					this->normals[counter]->Enable = true;
					counter++;
				}
			}
		}
	}
}

Editor *  Doom::Editor::Instance()
{
	static Editor instance;
	return &instance;
}