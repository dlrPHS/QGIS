/************************************************************************
 * This file has been generated automatically from                      *
 *                                                                      *
 * src/core/processing/qgsprocessingregistry.h                          *
 *                                                                      *
 * Do not edit manually ! Edit header and run scripts/sipify.pl again   *
 ************************************************************************/





class QgsProcessingRegistry : QObject
{
%Docstring
 Registry for various processing components, including providers, algorithms
 and various parameters and outputs.

 QgsProcessingRegistry is not usually directly created, but rather accessed through
 QgsApplication.processingRegistry().
.. versionadded:: 3.0
%End

%TypeHeaderCode
#include "qgsprocessingregistry.h"
%End
  public:

    QgsProcessingRegistry( QObject *parent /TransferThis/ = 0 );
%Docstring
 Constructor for QgsProcessingRegistry.
%End

    ~QgsProcessingRegistry();

//     QgsProcessingRegistry &operator=( const QgsProcessingRegistry &other ) = delete;
    QList<QgsProcessingProvider *> providers() const;
%Docstring
 Get list of available providers.
 :rtype: list of QgsProcessingProvider
%End

    bool addProvider( QgsProcessingProvider *provider /Transfer/ );
%Docstring
 Add a processing provider to the registry. Ownership of the provider is transferred to the registry,
 and the provider's parent will be set to the registry.
 Returns false if the provider could not be added (eg if a provider with a duplicate ID already exists
 in the registry).
 Adding a provider to the registry automatically triggers the providers QgsProcessingProvider.load()
 method to populate the provider with algorithms.
 \see removeProvider()
 :rtype: bool
%End

    bool removeProvider( QgsProcessingProvider *provider );
%Docstring
 Removes a provider implementation from the registry (the provider object is deleted).
 Returns false if the provider could not be removed (eg provider does not exist in the registry).
 \see addProvider()
 :rtype: bool
%End

    bool removeProvider( const QString &providerId );
%Docstring
 Removes a provider implementation from the registry (the provider object is deleted).
 Returns false if the provider could not be removed (eg provider does not exist in the registry).
 \see addProvider()
 :rtype: bool
%End

    QgsProcessingProvider *providerById( const QString &id );
%Docstring
 Returns a matching provider by provider ID.
 :rtype: QgsProcessingProvider
%End

    QList< const QgsProcessingAlgorithm *> algorithms() const;
%Docstring
 Returns a list of all available algorithms from registered providers.
 \see algorithmById()
 :rtype: list of const QgsProcessingAlgorithm
%End

    const QgsProcessingAlgorithm *algorithmById( const QString &id ) const;
%Docstring
 Finds an algorithm by its ID. If no matching algorithm is found, a None
 is returned.
 \see algorithms()
 :rtype: QgsProcessingAlgorithm
%End

  signals:

    void providerAdded( const QString &id );
%Docstring
Emitted when a provider has been added to the registry.
%End

    void providerRemoved( const QString &id );
%Docstring
Emitted when a provider is removed from the registry
%End

  private:
    QgsProcessingRegistry( const QgsProcessingRegistry &other );
};



/************************************************************************
 * This file has been generated automatically from                      *
 *                                                                      *
 * src/core/processing/qgsprocessingregistry.h                          *
 *                                                                      *
 * Do not edit manually ! Edit header and run scripts/sipify.pl again   *
 ************************************************************************/
