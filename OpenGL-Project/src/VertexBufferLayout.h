#pragma once
#include <vector>
#include <GL/glew.h>
#include "Renderer.h"

struct VertexBufferElement {
	unsigned int type;
	unsigned int count;
	bool normalized;

	static unsigned int GetSizeOfType(unsigned int type) {
		switch (type)
		{
		case GL_FLOAT:
		case GL_UNSIGNED_INT:
			return 4;
		case GL_UNSIGNED_BYTE:
			return 1;
		default:
			ASSERT(false);
			return 0;
		}
	}
};

class VertexBufferLayout
{
private:
	std::vector<VertexBufferElement> m_Elements;
	unsigned int m_Stride;
public:
	VertexBufferLayout()
		:m_Stride(0) {}


	// Could implement all types (GL_UNSIGNED_INT, GL_INT, GL_2_BYTES, etc.)
	
	// this is a function template
	// dictates that if called with a specific type (float, unsigned int, unsigned char), 
	// the templete will swap to that corresponding template to run
	
	template<typename T>
	void Push(unsigned int count) {
		static_assert(std::is_integral<T>, "Type must be an integral type");
	}

	template<>
	void Push<float>(unsigned int count) {
		m_Elements.push_back({ GL_FLOAT, count, GL_FALSE });
		m_Stride += VertexBufferElement::GetSizeOfType(GL_FLOAT) * count;
	}


	template<>
	void Push<unsigned int>(unsigned int count) {
		m_Elements.push_back({ GL_UNSIGNED_INT, count, GL_FALSE });
		m_Stride += VertexBufferElement::GetSizeOfType(GL_UNSIGNED_INT) * count;
	}

	template<>
	void Push<unsigned char>(unsigned int count) {
		m_Elements.push_back({ GL_UNSIGNED_BYTE, count, GL_TRUE });
		m_Stride += VertexBufferElement::GetSizeOfType(GL_UNSIGNED_BYTE) * count;
	}

	inline const std::vector<VertexBufferElement>& GetElements() const { return m_Elements; }

	inline unsigned int GetStride() const { return m_Stride; }
};


