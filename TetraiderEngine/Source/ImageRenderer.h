#pragma once

#ifndef IMAGE_RENDERER_H
#define IMAGE_RENDERER_H

class ImageRenderer
{
private:
	ShaderProgram* m_pShader;
	FrameBufferObject* m_pFBO;
	Mesh& m_mesh;

public:
	//ImageRenderer();
	//ImageRenderer(ShaderProgram* pShader, FrameBufferObject* pFBO);
	ImageRenderer();
	ImageRenderer(ShaderProgram*, GLsizei, GLsizei, FBOType);
	~ImageRenderer();
	ImageRenderer(const ImageRenderer &) = delete;
	void operator=(const ImageRenderer &) = delete;

	void Render(ImageRenderer * pIR) const;
	void Render(FrameBufferObject* pOtherFBO) const;

	/*
	Renders this Image Renderer's FBO to the screen with
	the supplied shader
	*/
	void RenderToScreen(const ShaderProgram&) const;

	/*
	Renders this Image Renderer's FBO + another IR's FBO to the screen with
	the supplied shader
	*/
	void RenderToScreen(const ShaderProgram&, const ImageRenderer&) const;

	/*
	Renders this Image Renderer's FBO to the supplied FBO with
	the supplied shader
	*/
	void RenderToFBO(const FrameBufferObject&, const ShaderProgram&) const;

	/*
	Renders this Image Renderer's FBO to the given Image Renderer's FBO
	with the supplied shader
	*/
	void RenderToIR(const ImageRenderer&, const ShaderProgram&) const;

	void BindFBO();
	void UnbindFBO();

	void ClearBuffer(const Vector3D& color = Vector3D(0,0,0,0));
};

#endif