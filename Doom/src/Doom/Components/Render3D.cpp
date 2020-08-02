#include "../pch.h"
#include "Render3D.h"
#include "../Core/Editor.h"
#include "../Render/Instancing.h"
#include "../Components/PointLight.h"

void Doom::Renderer3D::ChangeRenderTechnic(RenderTechnic rt)
{
	renderTechnic = rt;
	if (renderTechnic == RenderTechnic::Instancing) {
		for (auto i = Instancing::Instance()->instancedObjects.begin(); i != Instancing::Instance()->instancedObjects.end(); i++)
		{
			if (i->first == mesh)
				i->second.push_back(this);
		}
	}
}

void Doom::Renderer3D::LoadMesh(Mesh * _mesh)
{
	if (_mesh == nullptr)
		return;
	/*for (size_t i = 0; i < mesh->meshSize; i+=14)
	{
		Line* l = new Line(glm::vec3(0), glm::vec3(0));
		l->Enable = false;
		l->color = COLORS::Blue;
		Editor::Instance()->normals.push_back(l);
		Line::lines.pop_back();
	}*/
	mesh = _mesh;
}

Doom::Renderer3D::Renderer3D(GameObject* _owner)
{
	renderType = "3D";
	SetType(ComponentTypes::RENDER);
	owner = _owner;
	shader = Shader::Get("Default3D");
	tr = owner->GetComponentManager()->GetComponent<Transform>();
	pos = translate(glm::mat4(1.f), glm::vec3(tr->position.x, tr->position.y, tr->position.z));
}

Doom::Renderer3D::~Renderer3D()
{
}

#include "../Core/Timer.h"

void Doom::Renderer3D::Render()
{
	if (renderTechnic == RenderTechnic::Forward)
		ForwardRender();
}

void Doom::Renderer3D::ForwardRender()
{
	if (mesh != nullptr) {
		if (!isSkyBox) {
			shader->Bind();
			glBindTextureUnit(0, diffuseTexture->m_RendererID);
			shader->SetUniformMat4f("u_ViewProjection", Window::GetCamera().GetViewProjectionMatrix());
			shader->SetUniformMat4f("u_Model", pos);
			shader->SetUniformMat4f("u_View", view);
			shader->SetUniformMat4f("u_Scale", scale);
			shader->SetUniform4f("m_color", color[0], color[1], color[2], color[3]);

			int dlightSize = DirectionalLight::dirLights.size();
			shader->SetUniform1i("dLightSize", dlightSize);
			for (int i = 0; i < dlightSize; i++)
			{
				char buffer[64];
				DirectionalLight* dl = DirectionalLight::dirLights[i];
				dl->dir = dl->GetOwnerOfComponent()->GetComponent<Irenderer>()->view * glm::vec4(0, 0, -1, 1);
				sprintf(buffer, "dirLights[%i].dir", i);
				shader->SetUniform3fv(buffer, dl->dir);
				sprintf(buffer, "dirLights[%i].color", i);
				shader->SetUniform3fv(buffer, dl->color);
			}

			int plightSize = PointLight::pLights.size();
			shader->SetUniform1i("pLightSize", plightSize);
			char buffer[64];
			for (int i = 0; i < plightSize; i++)
			{
				PointLight* pl = PointLight::pLights[i];
				sprintf(buffer, "pointLights[%i].position", i);
				shader->SetUniform3fv(buffer, pl->GetOwnerOfComponent()->position);
				sprintf(buffer, "pointLights[%i].color", i);
				shader->SetUniform3fv(buffer, pl->color);
				sprintf(buffer, "pointLights[%i].constant", i);
				shader->SetUniform1f(buffer, pl->constant);
				sprintf(buffer, "pointLights[%i]._linear", i);
				shader->SetUniform1f(buffer, pl->linear);
				sprintf(buffer, "pointLights[%i].quadratic", i);
				shader->SetUniform1f(buffer, pl->quadratic);
			}

			shader->SetUniform3fv("u_CameraPos", Window::GetCamera().GetPosition());
			shader->SetUniform1f("u_Ambient", mat.ambient);
			shader->SetUniform1f("u_Specular", mat.specular);
			shader->SetUniform1i("u_DiffuseTexture", 0);
			if (useNormalMap) {
				glBindTextureUnit(1, normalMapTexture->m_RendererID);
				shader->SetUniform1i("u_NormalMapTexture", 1);
			}
			shader->SetUniform1i("u_isNormalMapping", useNormalMap);
			shader->Bind();
			mesh->va->Bind();
			mesh->ib->Bind();
			mesh->vb->Bind();
			Renderer::Vertices += mesh->meshSize * 0.5f;
			Renderer::DrawCalls++;
			glDrawElements(GL_TRIANGLES, mesh->ib->GetCount(), GL_UNSIGNED_INT, nullptr);
			shader->UnBind();
			mesh->ib->UnBind();
		}
		else {
			glDepthFunc(GL_LEQUAL);
			glActiveTexture(GL_TEXTURE0);
			glDisable(GL_CULL_FACE);
			glBindTexture(GL_TEXTURE_CUBE_MAP, diffuseTexture->m_RendererID);
			shader->Bind();
			Window::GetCamera().RecalculateViewMatrix();
			shader->SetUniformMat4f("u_ViewProjection", Window::GetCamera().GetProjectionMatrix());
			shader->SetUniformMat4f("u_Model", pos);
			shader->SetUniformMat4f("u_View", glm::mat4(glm::mat3(Window::GetCamera().GetViewMatrix())));
			shader->SetUniformMat4f("u_Scale", scale);
			shader->SetUniform1i("u_DiffuseTexture", 0);
			shader->Bind();
			mesh->va->Bind();
			mesh->ib->Bind();
			mesh->vb->Bind();
			Renderer::DrawCalls++;
			glDrawArrays(GL_TRIANGLES, 0, 36);
			glBindVertexArray(0);
			shader->UnBind();
			mesh->ib->UnBind();
			diffuseTexture->UnBind();
			glDepthFunc(GL_LESS);
			glEnable(GL_CULL_FACE);
		}
	}
}

