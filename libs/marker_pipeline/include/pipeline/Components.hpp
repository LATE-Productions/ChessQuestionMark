#pragma once
#include <algorithm>
#include <concepts>
#include <functional>
#include <memory>
#include <optional>
#include <type_traits>
#include <typeindex>
#include <unordered_map>
#include <utility>

#include "pipeline/Concepts.hpp"
namespace late {
namespace pipeline {
template <typename I, typename O>
class IStage {
 public:
  using In                               = I;
  using Out                              = O;
  virtual ~IStage()                      = default;
  virtual auto process(In&& data) -> Out = 0;
};

class Hooks {
 public:
  template <Stage T, typename Data, typename F>
    requires(std::is_same_v<Data, typename T::In> ||
             std::is_same_v<Data, typename T::Out>) &&
            HookFn<F, Data>
  auto add_hook(F&& fn) -> void {
    m_hooks[typeid(T)] = [func = std::forward<F>(fn)](const void* data) {
      func(*static_cast<const Data*>(data));
    };
  };
  auto exec_hook(std::type_index t, const void* data) -> void {
    if (m_hooks.contains(t)) {
      m_hooks[t](data);
    }
  };

 private:
  std::unordered_map<std::type_index, std::function<void(const void*)>>
      m_hooks{};
};

template <typename In, typename Out>
class Pipeline {
 public:
  using Output = std::optional<Out>;
  using Input  = In;
  template <Stage T, typename... Args>
    requires StageFrom<T, Out> && std::constructible_from<T, Args...>
  auto attach(Args&&... ctorArgs) && {
    auto stage = std::make_shared<T>(std::forward<Args>(ctorArgs)...);
    return Pipeline<In, typename T::Out>{
        [currrent_pipe = std::move(m_pipe), next_pipe = std::move(stage),
         in_hooks = m_inputHooks, out_hooks = m_outputHooks](In input) {
          Out payload = currrent_pipe(std::move(input));
          in_hooks->exec_hook(typeid(T), &payload);
          auto out = next_pipe->process(std::move(payload));
          out_hooks->exec_hook(typeid(T), &out);
          return out;
        },
        m_inputHooks, m_outputHooks
    };
  };

  template <Stage T, typename F>
    requires HookFn<F, typename T::Out>
  auto on_output(F&& fn) -> Pipeline& {
    m_outputHooks->template add_hook<T, typename T::Out>(std::move(fn));
    return *this;
  }

  template <Stage T, typename F>
    requires HookFn<F, typename T::In>
  auto on_input(F&& fn) -> Pipeline& {
    m_inputHooks->template add_hook<T, typename T::In>(std::move(fn));
    return *this;
  }

  template <typename Input>
    requires PipeInput<Input, In>
  auto operator<<(Input&& data) -> Pipeline& {
    m_input.emplace(std::forward<Input>(data));
    return *this;
  };

  auto operator>>(std::optional<Out>& output) -> Pipeline& {
    if (!m_input) {
      output = std::nullopt;
    } else {
      output = m_pipe(std::move(*m_input));
      m_input.reset();
    }
    return *this;
  }

  Pipeline(
      std::function<Out(In)> pipe, std::shared_ptr<Hooks> in_hooks,
      std::shared_ptr<Hooks> out_hooks
  )
      : m_pipe(pipe),
        m_inputHooks(in_hooks),
        m_outputHooks(out_hooks),
        m_input{} {};

 private:
  std::function<Out(In)> m_pipe;
  std::shared_ptr<Hooks> m_inputHooks;
  std::shared_ptr<Hooks> m_outputHooks;
  std::optional<In> m_input;
};

template <typename T>
auto make_pipeline() -> Pipeline<T, T> {
  return Pipeline<T, T>{
      [](T in) { return in; }, std::make_shared<Hooks>(),
      std::make_shared<Hooks>()
  };
}
}  // namespace pipeline
}  // namespace late
