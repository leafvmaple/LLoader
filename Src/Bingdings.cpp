#include <pybind11/pybind11.h>
#include <pybind11/stl.h>
#include "IAnimation.h"

namespace py = pybind11;

PYBIND11_MODULE(L3DEngine, m) {
	py::enum_<ANIMATION_TYPE>(m, "ANIMATION_TYPE")
		.value("ANIMATION_NONE", ANIMATION_TYPE::ANIMATION_NONE)
		.value("ANIMATION_VERTICES", ANIMATION_TYPE::ANIMATION_VERTICES)
		.value("ANIMATION_VERTICES_RELATIVE", ANIMATION_TYPE::ANIMATION_VERTICES_RELATIVE)
		.value("ANIMATION_BONE", ANIMATION_TYPE::ANIMATION_BONE)
		.value("ANIMATION_BONE_RELATIVE", ANIMATION_TYPE::ANIMATION_BONE_RELATIVE)
		.value("ANIMATION_BONE_16", ANIMATION_TYPE::ANIMATION_BONE_16)
		.value("ANIMATION_BONE_RTS", ANIMATION_TYPE::ANIMATION_BONE_RTS)
		.value("ANIMATION_BLENDSHAPE", ANIMATION_TYPE::ANIMATION_BLENDSHAPE)
		.value("ANIMATION_VCIK", ANIMATION_TYPE::ANIMATION_VCIK)
		.value("ANIMATION_COUNT", ANIMATION_TYPE::ANIMATION_COUNT)
		.value("ANIMATION_FORCE_DWORD", ANIMATION_TYPE::ANIMATION_FORCE_DWORD);

	py::enum_<BONE_FLAG>(m, "BONE_FLAG")
		.value("BONE_FLAG_NONE", BONE_FLAG::BONE_FLAG_NONE)
		.value("BONE_FLAG_AFFLINE", BONE_FLAG::BONE_FLAG_AFFLINE)
		.value("BONE_FLAG_NO_UPDATE", BONE_FLAG::BONE_FLAG_NO_UPDATE);

	py::class_<ANIMATION_DESC>(m, "ANIMATION_DESC")
		.def(py::init<>())
		.def_readwrite("szFileName", &ANIMATION_DESC::szFileName);

	py::class_<XMFLOAT3>(m, "XMFLOAT3")
		.def(py::init<>())
		.def_readwrite("x", &XMFLOAT3::x)
		.def_readwrite("y", &XMFLOAT3::y)
		.def_readwrite("z", &XMFLOAT3::z);

	py::class_<XMFLOAT4>(m, "XMFLOAT4")
		.def(py::init<>())
		.def_readwrite("x", &XMFLOAT4::x)
		.def_readwrite("y", &XMFLOAT4::y)
		.def_readwrite("z", &XMFLOAT4::z)
		.def_readwrite("w", &XMFLOAT4::w);

	py::class_<RTS>(m, "RTS")
		.def(py::init<>())
		.def_readwrite("Translation", &RTS::Translation)
		.def_readwrite("Scale", &RTS::Scale)
		.def_readwrite("Rotation", &RTS::Rotation)
		.def_readwrite("Sign", &RTS::Sign)
		.def_readwrite("SRotation", &RTS::SRotation);

	py::class_<ANIMATION_SOURCE, LUnknown>(m, "ANIMATION_SOURCE")
		.def(py::init<>())
		.def_readwrite("nAnimationType", &ANIMATION_SOURCE::nAnimationType)
		.def_readwrite("nBonesCount", &ANIMATION_SOURCE::nBonesCount)
		.def_readwrite("nFrameCount", &ANIMATION_SOURCE::nFrameCount)
		.def_readwrite("fFrameLength", &ANIMATION_SOURCE::fFrameLength)
		.def_readwrite("nAnimationLength", &ANIMATION_SOURCE::nAnimationLength)
		.def_readwrite("pBoneNames", &ANIMATION_SOURCE::pBoneNames)
		.def_readwrite("pBoneRTS", &ANIMATION_SOURCE::pBoneRTS)
		.def_readwrite("pFlag", &ANIMATION_SOURCE::pFlag);

	m.def("LoadAnimation", [](const ANIMATION_DESC& desc) {
		ANIMATION_SOURCE* source = nullptr;
		LoadAnimation(const_cast<ANIMATION_DESC*>(&desc), source);
		return std::unique_ptr<ANIMATION_SOURCE>(source);
		});
}