package pl.tzr.transparent;

import pl.tzr.transparent.structure.model.ModelRegistry;

public class DatabaseContext {
	
	private ModelRegistry modelRegistry;
	
	private TransparentProxyFactory transparentProxyFactory;

	public ModelRegistry getModelRegistry() {
		return modelRegistry;
	}

	public void setModelRegistry(ModelRegistry modelRegistry) {
		this.modelRegistry = modelRegistry;
	}

	public TransparentProxyFactory getTransparentProxyFactory() {
		return transparentProxyFactory;
	}

	public void setTransparentProxyFactory(
			TransparentProxyFactory transparentProxyFactory) {
		this.transparentProxyFactory = transparentProxyFactory;
	}

}
