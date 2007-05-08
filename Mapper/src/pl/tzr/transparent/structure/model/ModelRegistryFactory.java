package pl.tzr.transparent.structure.model;

/**
 * Fabryka modelu danych. Implementacje tego interfejsu potrafią
 * stworzyć model danych
 * @author Tomasz Rosiek
 *
 */
public interface ModelRegistryFactory {

	ModelRegistry getModelRegistry();
	
}
