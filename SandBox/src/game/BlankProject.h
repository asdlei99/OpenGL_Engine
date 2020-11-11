#pragma once

class Obj : public GameObject {
public:
	float u_EdgeThickness = 2;
	float u_EdgeSharpness = 30;
	float u_EdgeSubstruct = 0.3;
	float u_GlowStrength = 10;
	Obj() : GameObject() {
		GetComponentManager()->AddComponent<Renderer3D>()->LoadMesh(MeshManager::GetMesh("cube"));
		GetComponent<Renderer3D>()->shader = Shader::Get("Tron");
		Renderer3D* r = GetComponentManager()->GetComponent<Renderer3D>();
		r->floatUniforms.insert(std::make_pair("u_EdgeThickness", u_EdgeThickness));
		r->floatUniforms.insert(std::make_pair("u_EdgeSharpness", u_EdgeSharpness));
		r->floatUniforms.insert(std::make_pair("u_EdgeSubstruct", u_EdgeSubstruct));
		r->floatUniforms.insert(std::make_pair("u_GlowStrength", u_GlowStrength));
	}
};

#include <numeric>
class BlankProject : public Application {
public:
	Obj* go = nullptr;
	Shader* shader = nullptr;
	double time = 0;
	GameObject* earth;
	GameObject* moon;
	BlankProject(std::string name = "SandBox", int width = 800, int height = 600, bool Vsync = false) : Application(name, TYPE_3D, width, height, Vsync) {}
	std::vector<std::string> faces = {
				"src/SkyBox/right.png",
				"src/SkyBox/left.png",
				"src/SkyBox/top.png",
				"src/SkyBox/bottom.png",
				"src/SkyBox/front.png",
				"src/SkyBox/back.png",


	};
	virtual void OnStart() {
		ImGui::SetCurrentContext(Window::imGuiContext);
		//shader = Shader::Get("Tron");
		//GameObject* dirLight = new GameObject("Directional Light", 0, 0, 0);
		//dirLight->GetComponentManager()->AddComponent<DirectionalLight>();
		//dirLight->GetComponent<Transform>()->RotateOnce(-50,0,0);
		//SkyBox* skybox = new SkyBox(faces, nullptr);
		//MeshManager::GetMeshWhenLoaded("cube",(void*)(skybox->GetComponentManager()->GetComponent<Renderer3D>()));
		//Gui::GetInstance()->FontBind(Gui::GetInstance()->GetStandartFonts()[Gui::StandartFonts::ARIAL]);
		//earth = new GameObject("Earth", 0, 0, 0);
		//earth->transform->Scale(18, 18, 18);
		//moon = new GameObject("Moon", 0, 6, 0);
		//earth->transform->RotateOnce(70, 0, 0);
		//moon->transform->RotateOnce(70, 0, 0);
		//moon->transform->Scale(5, 5, 5);
		//moon->GetComponentManager()->AddComponent<PointLight>();
		//MeshManager::GetMeshWhenLoaded("sphere", earth->GetComponentManager()->AddComponent<Renderer3D>());
		//MeshManager::GetMeshWhenLoaded("sphere", moon->GetComponentManager()->AddComponent<Renderer3D>());
		//GameObject* terrain = new GameObject("Terrain");
		//for (int i = -2; i < 4; i++)
		//{
		//	for (int j = -2; j < 4; j++)
		//	{
		//		go = new Obj();
		//		go->SetOwner((void*)terrain);
		//		terrain->AddChild((void*)go);
		//		go->transform->Translate(i * 2, 0, j * 2);
		//		go->name = std::to_string(i) + " " + std::to_string(j);
		//		go->GetComponent<Renderer3D>()->mat.ambient = 0.7;
		//		go->GetComponent<Renderer3D>()->mat.specular = 1;
		//	}
		//}
		//Texture::GetAsync(earth, [=] {
		//	Texture* t = Texture::Get("src/Images/earth.png");
		//	if (t != nullptr)
		//		earth->GetComponentManager()->GetComponent<Renderer3D>()->diffuseTexture = t;
		//	return t;
		//});
	}

	virtual void OnUpdate() {
		//earth->transform->Rotate( 0, 0, 8);
		//moon->transform->Rotate(0, 0, -30);
		//moon->transform->Translate(glm::sin(time) * 30, 6, glm::cos(time) * 30);
		//time += DeltaTime::deltatime;
	}

	virtual void OnGuiRender() {
		Gui* g = Gui::GetInstance();
		g->xAlign = g->XCENTER;
		g->Text("FPS %f",1,850,500,40,COLORS::White,3,Window::GetFPS());
	}

};