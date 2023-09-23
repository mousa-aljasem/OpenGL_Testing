#pragma once

#include <vector>
#include "Renderer.h"

struct VertexBufferElement {
	unsigned int type;
	unsigned int count;
	unsigned char normalized;

	static unsigned int GetSizeOfType(unsigned int type) {
		switch (type) {
			case GL_FLOAT:			return sizeof(GLfloat);
			case GL_UNSIGNED_INT:	return sizeof(GLuint);
			case GL_UNSIGNED_BYTE:	return sizeof(GLbyte);
		}
		ASSERT(false);
		return 0;
	}
};

class VertexBufferLayout {
private:
	std::vector<VertexBufferElement> m_Elements;
	unsigned int m_Stride;
public:
	VertexBufferLayout() : m_Stride(0) {}

	template<typename T>
	void Push(unsigned int count) {
		static_assert(false);
	}

	template<>
	void Push<float>(unsigned int count) {
		Push(GL_FLOAT, count, GL_FALSE);
	}

	template<>
	void Push<unsigned int>(unsigned int count) {
		Push(GL_UNSIGNED_INT, count, GL_FALSE);
	}

	template<>
	void Push<unsigned char>(unsigned int count) {
		Push(GL_UNSIGNED_BYTE, count, GL_TRUE);
	}

	inline const std::vector<VertexBufferElement> GetElements() const { return m_Elements; }
	inline unsigned int GetStride() const { return m_Stride; }

private:
	void Push(unsigned int type, unsigned int count, unsigned char normalized) {
		m_Elements.push_back({ type, count, normalized });
		m_Stride += count * VertexBufferElement::GetSizeOfType(type);
	}
};