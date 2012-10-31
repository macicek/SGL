#ifndef CONTEXT_MANAGER_H
#define CONTEXT_MANAGER_H

class ContextManager
{
	public:
		Context* currentContext(){ return _currentContext; }
		uint32 addContext(Context* context)
		{
			_contextContainer.push_back(context);

			return _contextContainer.size()-1;
		}
		void setCurrentContext(Context* c){ _currentContext = c; }
		void setCurrentContext(uint32 id){ _currentContext = _contextContainer[id]; }

		uint32 contextId(){ return _contextContainer.size()-1; }
		uint32 contextSize(){ return _contextContainer.size(); }

		void destroyContext(uint32 id)
		{
			delete _contextContainer[id];

			_contextContainer[id] = 0;
		} 

	private:
		Context* _currentContext;
		std::vector<Context*> _contextContainer;
};

#endif
