#include "../pch.h"
#include "Renderer2D.h"

using namespace Doom;

void Renderer2DLayer::OnRunning(OrthographicCamera& camera){

}

void Renderer2DLayer::Draw(const VertexArray& va, const IndexBuffer& ib, const Shader& shader) {
	va.Bind();
	ib.Bind();
	glDrawElements(GL_TRIANGLES,ib.GetCount(), GL_UNSIGNED_INT,nullptr);
}

void Renderer2DLayer::Setlayer(int layer)
{
	unsigned int size = Renderer2DLayer::objects2d.size();
	std::cout << *name << " is set from layer " << this->GetId() << " to " << layer << std::endl;
	Renderer2DLayer* tmp = this;
	Renderer2DLayer::objects2d.erase(objects2d.begin() + this->GetId());
	Renderer2DLayer::objects2d.insert(objects2d.begin() + layer,*tmp);
	for (unsigned int i = 0; i < objects2d.size(); i++)
	{
		objects2d[i].get().SetId(i);
		objects2d[i].get().GetLayer() = i;
	}
	return;
}

const char** Renderer2DLayer::GetItemsNames() {
	if (items != nullptr)
		delete[] items;
	items = new const char*[Childs.size()];
	for (unsigned int i = 0; i < Childs.size(); i++)
	{
		items[i] = static_cast<Renderer2DLayer*>(Childs[i])->name->c_str();
	}
	return items;
}
