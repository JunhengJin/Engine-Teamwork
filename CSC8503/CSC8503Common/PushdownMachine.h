#pragma once
#include <stack>

namespace NCL {
	namespace CSC8503 {
		class PushdownState;

		class PushdownMachine
		{
		public:
			PushdownMachine(PushdownState* initialState)
			{
				this->initialState = initialState;
			}
			~PushdownMachine() {}

			bool Update(float dt);

			PushdownState* GetActiveState()
			{
				return activeState;
			}
			void SetActiveState(PushdownState* state)
			{
				activeState = state;
			}

			std::stack<PushdownState*> GetStateStack()
			{
				return stateStack;
			}
			void PushStateStack(PushdownState* state)
			{
				stateStack.push(state);
			}
			void PopStateStack(int times)
			{
				int t = times;
				for (int i = 0; i < t; i++)
				{
					delete activeState;
					stateStack.pop();
					if (stateStack.empty())
					{
						return;
					}
					else
					{
						activeState = stateStack.top();
					}
				}

			}

			void ClearStateStack()
			{
				if (stateStack.empty())
					return;

				for (int i = 0; i < stateStack.size() - 1; i++)
				{
					stateStack.pop();
				}
			}

		protected:
			PushdownState* activeState;
			PushdownState* initialState;
			std::stack<PushdownState*> stateStack;
		};
	}
}

