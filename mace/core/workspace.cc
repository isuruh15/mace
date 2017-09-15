//
// Copyright (c) 2017 XiaoMi All rights reserved.
//

#include "mace/core/workspace.h"
#include "mace/core/common.h"
#include "mace/core/serializer.h"

namespace mace {

vector<string> Workspace::Tensors() const {
  vector<string> names;
  for (auto& entry : tensor_map_) {
    names.push_back(entry.first);
  }
  return names;
}

Tensor* Workspace::CreateTensor(const string& name,
                                Allocator* alloc,
                                DataType type) {
  if (HasTensor(name)) {
    VLOG(1) << "Tensor " << name << " already exists. Skipping.";
  } else {
    VLOG(1) << "Creating Tensor " << name;
    tensor_map_[name] = unique_ptr<Tensor>(new Tensor(alloc, type));
  }
  return GetTensor(name);
}

bool Workspace::RemoveTensor(const string& name) {
  auto it = tensor_map_.find(name);
  if (it != tensor_map_.end()) {
    VLOG(1) << "Removing blob " << name << " from this workspace.";
    tensor_map_.erase(it);
    return true;
  }
  return false;
}

const Tensor* Workspace::GetTensor(const string& name) const {
  if (tensor_map_.count(name)) {
    return tensor_map_.at(name).get();
  }
  return nullptr;
}

Tensor* Workspace::GetTensor(const string& name) {
  return const_cast<Tensor*>(
      static_cast<const Workspace*>(this)->GetTensor(name));
}

void Workspace::LoadModelTensor(const NetDef& net_def, DeviceType type) {
  Serializer serializer;
  for (auto& tensor_proto : net_def.tensors()) {
    tensor_map_[tensor_proto.name()] =
        serializer.Deserialize(tensor_proto, type);
  }
}

}  // namespace mace