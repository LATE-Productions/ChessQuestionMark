#pragma once

#include <algorithm>
#include <concepts>

template <typename T>
concept Stage = requires {
  typename T::In;
  typename T::Out;
} && requires(T& stage, typename T::In input) {
  { stage.process(std::move(input)) } -> std::convertible_to<typename T::Out>;
};

template <typename T, typename In>
concept StageFrom = Stage<T> && std::same_as<typename T::In, In>;

template <typename T, typename Data>
concept HookFn = std::invocable<T, const Data&>;

template <typename Input, typename In>
concept PipeInput = std::constructible_from<In, Input&&>;
