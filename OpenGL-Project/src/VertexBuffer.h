#pragma once
class VertexBuffer
{
private:
	unsigned int m_VertexBufferID;
public:
	VertexBuffer(const void* buffer, unsigned int size);
	~VertexBuffer();

	void Bind() const;
	void Unbind() const;
};

