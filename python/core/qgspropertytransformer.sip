/************************************************************************
 * This file has been generated automatically from                      *
 *                                                                      *
 * src/core/qgspropertytransformer.h                                    *
 *                                                                      *
 * Do not edit manually ! Edit header and run scripts/sipify.pl again   *
 ************************************************************************/





class QgsCurveTransform
{
%Docstring
 Handles scaling of input values to output values by using a curve created
 from smoothly joining a number of set control points.

 QgsCurveTransform assists in creation of curve type transforms, typically seen in
 raster image editing software (eg the curves dialog in GIMP or Photoshop).
 Transforms are created by passing a number of set control points through which
 the transform curve must pass. The curve is guaranteed to exactly pass through
 these control points. Between control points the curve is smoothly interpolated
 so that no disjoint sections or "corners" are present.

 If the first or last control point are not located at x = 0 and x = 1 respectively,
 then values outside this range will be mapped to the y value of either the first
 or last control point. In other words, the curve will have a flat segment
 for values outside of the control point range.

.. versionadded:: 3.0
%End

%TypeHeaderCode
#include "qgspropertytransformer.h"
%End
  public:

    QgsCurveTransform();
%Docstring
 Constructs a default QgsCurveTransform which linearly maps values
 between 0 and 1 unchanged. I.e. y == x.
%End

    QgsCurveTransform( const QList< QgsPoint > &controlPoints );
%Docstring
 Constructs a QgsCurveTransform using a specified list of ``controlPoints``.
 Behavior is undefined if duplicate x values exist in the control points
 list.
%End

    ~QgsCurveTransform();

    QgsCurveTransform( const QgsCurveTransform &other );
%Docstring
 Copy constructor
%End

//     QgsCurveTransform &operator=( const QgsCurveTransform &other );
    QList< QgsPoint > controlPoints() const;
%Docstring
 Returns a list of the control points for the transform.
 \see setControlPoints()
 :rtype: list of QgsPoint
%End

    void setControlPoints( const QList< QgsPoint > &points );
%Docstring
 Sets the list of control points for the transform. Any existing
 points are removed.
 \see controlPoints()
%End

    void addControlPoint( double x, double y );
%Docstring
 Adds a control point to the transform. Behavior is undefined if duplicate
 x values exist in the control points list.
 \see removeControlPoint()
%End

    void removeControlPoint( double x, double y );
%Docstring
 Removes a control point from the transform. This will have no effect if a
 matching control point does not exist.
 \see addControlPoint()
%End

    double y( double x ) const;
%Docstring
 Returns the mapped y value corresponding to the specified ``x`` value.
 :rtype: float
%End

    QVector< double > y( const QVector< double > &x ) const;
%Docstring
 Returns a list of y values corresponding to a list of ``x`` values.
 Calling this method is faster then calling the double variant multiple
 times.
 :rtype: list of float
%End

    bool readXml( const QDomElement &elem, const QDomDocument &doc );
%Docstring
 Reads the curve's state from an XML element.
 \param elem source DOM element for transform's state
 \param doc DOM document
 \see writeXml()
 :rtype: bool
%End

    bool writeXml( QDomElement &transformElem, QDomDocument &doc ) const;
%Docstring
 Writes the current state of the transform into an XML element
 \param transformElem destination element for the transform's state
 \param doc DOM document
 \see readXml()
 :rtype: bool
%End

    QVariant toVariant() const;
%Docstring
 Saves this curve transformer to a QVariantMap, wrapped in a QVariant.
 You can use QgsXmlUtils.writeVariant to save it to an XML document.

 \see loadVariant()
 :rtype: QVariant
%End

    bool loadVariant( const QVariant &transformer );
%Docstring
 Load this curve transformer from a QVariantMap, wrapped in a QVariant.
 You can use QgsXmlUtils.writeVariant to load it from an XML document.

 \see toVariant()
 :rtype: bool
%End

};


class QgsPropertyTransformer
{
%Docstring
 Abstract base class for objects which transform the calculated value of a property.
 Possible uses include transformers which map a value into a scaled size or color from a gradient.
.. versionadded:: 3.0
%End

%TypeHeaderCode
#include "qgspropertytransformer.h"
%End
  private:
%ConvertToSubClassCode
    if ( sipCpp->transformerType() == QgsPropertyTransformer::GenericNumericTransformer )
      sipType = sipType_QgsGenericNumericTransformer;
    else if ( sipCpp->transformerType() == QgsPropertyTransformer::SizeScaleTransformer )
      sipType = sipType_QgsSizeScaleTransformer;
    else if ( sipCpp->transformerType() == QgsPropertyTransformer::ColorRampTransformer )
      sipType = sipType_QgsColorRampTransformer;
    else
      sipType = sipType_QgsPropertyTransformer;
%End
  public:

    enum Type
    {
      GenericNumericTransformer,
      SizeScaleTransformer,
      ColorRampTransformer,
    };

    static QgsPropertyTransformer *create( Type type ) /Factory/;
%Docstring
 Factory method for creating a new property transformer of the specified type.
 \param type transformer type to create
 :rtype: QgsPropertyTransformer
%End

    QgsPropertyTransformer( double minValue = 0.0, double maxValue = 1.0 );
%Docstring
 Constructor for QgsPropertyTransformer
 \param minValue minimum expected value from source property
 \param maxValue maximum expected value from source property
%End

    QgsPropertyTransformer( const QgsPropertyTransformer &other );
%Docstring
 Copy constructor.
%End
//     QgsPropertyTransformer &operator=( const QgsPropertyTransformer &other );
    virtual ~QgsPropertyTransformer();

    virtual Type transformerType() const = 0;
%Docstring
 Returns the transformer type.
 :rtype: Type
%End

    virtual QgsPropertyTransformer *clone() = 0 /Factory/;
%Docstring
 Returns a clone of the transformer.
 :rtype: QgsPropertyTransformer
%End

    virtual bool loadVariant( const QVariant &transformer );
%Docstring
 Loads this transformer from a QVariantMap, wrapped in a QVariant.
 You can use QgsXmlUtils.writeVariant to save it to an XML document.

 \see loadVariant()
 :rtype: bool
%End

    virtual QVariant toVariant() const;
%Docstring
 Saves this transformer to a QVariantMap, wrapped in a QVariant.
 You can use QgsXmlUtils.writeVariant to save it to an XML document.

 \see toVariant()
 :rtype: QVariant
%End

    double minValue() const;
%Docstring
 Returns the minimum value expected by the transformer.
 \see maxValue()
 \see setMinValue()
 :rtype: float
%End

    void setMinValue( double min );
%Docstring
 Sets the minimum value expected by the transformer.
 \param min minimum value
 \see setMaxValue()
 \see minValue()
%End

    double maxValue() const;
%Docstring
 Returns the maximum value expected by the transformer.
 \see minValue()
 \see setMaxValue()
 :rtype: float
%End

    void setMaxValue( double max );
%Docstring
 Sets the maximum value expected by the transformer.
 \param max maximum value
 \see setMinValue()
 \see maxValue()
%End

    QgsCurveTransform *curveTransform() const;
%Docstring
 Returns the curve transform applied to input values before they are transformed
 by the individual transform subclasses.
 \see setCurveTransform()
 :rtype: QgsCurveTransform
%End

    void setCurveTransform( QgsCurveTransform *transform /Transfer/ );
%Docstring
 Sets a curve transform to apply to input values before they are transformed
 by the individual transform subclasses. Ownership of ``transform`` is transferred
 to the property transformer.
 \see curveTransform()
%End

    virtual QVariant transform( const QgsExpressionContext &context, const QVariant &value ) const = 0;
%Docstring
 Calculates the transform of a value. Derived classes must implement this to perform their transformations
 on input values
 \param context expression context
 \param value input value to transform
 :rtype: QVariant
%End

    virtual QString toExpression( const QString &baseExpression ) const = 0;
%Docstring
 Converts the transformer to a QGIS expression string. The ``baseExpression`` string consists
 of a sub-expression reflecting the parent property's state.
 :rtype: str
%End

    static QgsPropertyTransformer *fromExpression( const QString &expression, QString &baseExpression /Out/, QString &fieldName /Out/ ) /Factory/;
%Docstring
 Attempts to parse an expression into a corresponding property transformer.
 \param expression expression to parse
 \param baseExpression will be set to the component of the source expression which
 is used to calculate the input to the property transformer. This will be set to an
 empty string if a field reference is the transformer input.
 \param fieldName will be set to a field name which is used to calculate the input
 to the property transformer. This will be set to an
 empty string if an expression is the transformer input.
 :return: corresponding property transformer, or None if expression could not
 be parsed to a transformer.
 :rtype: QgsPropertyTransformer
%End

  protected:

    double transformNumeric( double input ) const;
%Docstring
 Applies base class numeric transformations. Derived classes should call this
 to transform an ``input`` numeric value before they apply any transform to the result.
 This applies any curve transforms which may exist on the transformer.
 :rtype: float
%End
};


class QgsGenericNumericTransformer : QgsPropertyTransformer
{
%Docstring
 QgsPropertyTransformer subclass for scaling an input numeric value into an output numeric value.
.. versionadded:: 3.0
%End

%TypeHeaderCode
#include "qgspropertytransformer.h"
%End
  public:

    QgsGenericNumericTransformer( double minValue = 0.0,
                                  double maxValue = 1.0,
                                  double minOutput = 0.0,
                                  double maxOutput = 1.0,
                                  double nullOutput = 0.0,
                                  double exponent = 1.0 );
%Docstring
 Constructor for QgsGenericNumericTransformer.
 \param minValue minimum expected input value
 \param maxValue maximum expected input value
 \param minOutput minimum value to return
 \param maxOutput maximum value to return
 \param nullOutput value to return for null inputs
 \param exponent optional exponential for non-linear scaling
%End

    QgsGenericNumericTransformer( const QgsGenericNumericTransformer &other );
%Docstring
 Copy constructor.
%End
//     QgsGenericNumericTransformer &operator=( const QgsGenericNumericTransformer &other );
    virtual Type transformerType() const;
    virtual QgsGenericNumericTransformer *clone();
    virtual QVariant toVariant() const;
    virtual bool loadVariant( const QVariant &definition );
    virtual QVariant transform( const QgsExpressionContext &context, const QVariant &value ) const;
    virtual QString toExpression( const QString &baseExpression ) const;

    static QgsGenericNumericTransformer *fromExpression( const QString &expression, QString &baseExpression /Out/, QString &fieldName /Out/ ) /Factory/;
%Docstring
 Attempts to parse an expression into a corresponding QgsSizeScaleTransformer.
 \param expression expression to parse
 \param baseExpression will be set to the component of the source expression which
 is used to calculate the input to the property transformer. This will be set to an
 empty string if a field reference is the transformer input.
 \param fieldName will be set to a field name which is used to calculate the input
 to the property transformer. This will be set to an
 empty string if an expression is the transformer input.
 :return: corresponding QgsSizeScaleTransformer, or None if expression could not
 be parsed to a size scale transformer.
 :rtype: QgsGenericNumericTransformer
%End

    double value( double input ) const;
%Docstring
 Calculates the size corresponding to a specific ``input`` value.
 :return: calculated size using size scale transformer's parameters and type
 :rtype: float
%End

    double minOutputValue() const;
%Docstring
 Returns the minimum calculated size.
 \see setMinSize()
 \see maxSize()
 :rtype: float
%End

    void setMinOutputValue( double size );
%Docstring
 Sets the minimum calculated size.
 \param size minimum size
 \see minSize()
 \see setMaxSize()
%End

    double maxOutputValue() const;
%Docstring
 Returns the maximum calculated size.
 \see minSize()
 :rtype: float
%End

    void setMaxOutputValue( double size );
%Docstring
 Sets the maximum calculated size.
 \param size maximum size
 \see maxSize()
 \see setMinSize()
%End

    double nullOutputValue() const;
%Docstring
 Returns the size value when an expression evaluates to NULL.
 \see setNullSize()
 :rtype: float
%End

    void setNullOutputValue( double size );
%Docstring
 Sets the size value for when an expression evaluates to NULL.
 \param size null size
 \see nullSize()
%End

    double exponent() const;
%Docstring
 Returns the exponent for an exponential expression.
 \see setExponent()
 \see type()
 :rtype: float
%End

    void setExponent( double exponent );
%Docstring
 Sets the exponent for an exponential expression.
 \param exponent exponent
 \see exponent()
%End

};


class QgsSizeScaleTransformer : QgsPropertyTransformer
{
%Docstring
 QgsPropertyTransformer subclass for scaling a value into a size according to various
 scaling methods.
.. versionadded:: 3.0
%End

%TypeHeaderCode
#include "qgspropertytransformer.h"
%End
  public:

    enum ScaleType
    {
      Linear,
      Area,
      Flannery,
      Exponential,
    };

    QgsSizeScaleTransformer( ScaleType type = Linear,
                             double minValue = 0.0,
                             double maxValue = 1.0,
                             double minSize = 0.0,
                             double maxSize = 1.0,
                             double nullSize = 0.0,
                             double exponent = 1.0 );
%Docstring
 Constructor for QgsSizeScaleTransformer.
 \param type scaling type
 \param minValue minimum expected value
 \param maxValue maximum expected value
 \param minSize minimum size to return
 \param maxSize maximum size to return
 \param nullSize size to return for null values
 \param exponent exponent for Exponential scaling method
%End

    QgsSizeScaleTransformer( const QgsSizeScaleTransformer &other );
%Docstring
 Copy constructor.
%End
//     QgsSizeScaleTransformer &operator=( const QgsSizeScaleTransformer &other );
    virtual Type transformerType() const;
    virtual QgsSizeScaleTransformer *clone() /Factory/;
    virtual QVariant toVariant() const;
    virtual bool loadVariant( const QVariant &definition );
    virtual QVariant transform( const QgsExpressionContext &context, const QVariant &value ) const;
    virtual QString toExpression( const QString &baseExpression ) const;

    static QgsSizeScaleTransformer *fromExpression( const QString &expression, QString &baseExpression /Out/, QString &fieldName /Out/ ) /Factory/;
%Docstring
 Attempts to parse an expression into a corresponding QgsSizeScaleTransformer.
 \param expression expression to parse
 \param baseExpression will be set to the component of the source expression which
 is used to calculate the input to the property transformer. This will be set to an
 empty string if a field reference is the transformer input.
 \param fieldName will be set to a field name which is used to calculate the input
 to the property transformer. This will be set to an
 empty string if an expression is the transformer input.
 :return: corresponding QgsSizeScaleTransformer, or None if expression could not
 be parsed to a size scale transformer.
 :rtype: QgsSizeScaleTransformer
%End

    double size( double value ) const;
%Docstring
 Calculates the size corresponding to a specific value.
 \param value value to calculate size for
 :return: calculated size using size scale transformer's parameters and type
 :rtype: float
%End

    double minSize() const;
%Docstring
 Returns the minimum calculated size.
 \see setMinSize()
 \see maxSize()
 :rtype: float
%End

    void setMinSize( double size );
%Docstring
 Sets the minimum calculated size.
 \param size minimum size
 \see minSize()
 \see setMaxSize()
%End

    double maxSize() const;
%Docstring
 Returns the maximum calculated size.
 \see minSize()
 :rtype: float
%End

    void setMaxSize( double size );
%Docstring
 Sets the maximum calculated size.
 \param size maximum size
 \see maxSize()
 \see setMinSize()
%End

    double nullSize() const;
%Docstring
 Returns the size value when an expression evaluates to NULL.
 \see setNullSize()
 :rtype: float
%End

    void setNullSize( double size );
%Docstring
 Sets the size value for when an expression evaluates to NULL.
 \param size null size
 \see nullSize()
%End

    double exponent() const;
%Docstring
 Returns the exponent for an exponential expression.
 \see setExponent()
 \see type()
 :rtype: float
%End

    void setExponent( double exponent );
%Docstring
 Sets the exponent for an exponential expression.
 \param exponent exponent
 \see exponent()
%End

    ScaleType type() const;
%Docstring
 Returns the size transformer's scaling type (the method used to calculate
 the size from a value).
 \see setType()
 :rtype: ScaleType
%End

    void setType( ScaleType type );
%Docstring
 Sets the size transformer's scaling type (the method used to calculate
 the size from a value).
 \param type scale type
 \see type()
%End

};


class QgsColorRampTransformer : QgsPropertyTransformer
{
%Docstring
 QgsPropertyTransformer subclass for transforming a numeric value into a color from a
 color ramp.
.. versionadded:: 3.0
%End

%TypeHeaderCode
#include "qgspropertytransformer.h"
%End
  public:

    QgsColorRampTransformer( double minValue = 0.0,
                             double maxValue = 1.0,
                             QgsColorRamp *ramp /Transfer/ = 0,
                             const QColor &nullColor = QColor( 0, 0, 0, 0 ) );
%Docstring
 Constructor for QgsColorRampTransformer.
 \param minValue minimum expected value
 \param maxValue maximum expected value
 \param ramp source color ramp. Ownership is transferred to the transformer.
 \param nullColor color to return for null values
%End

    QgsColorRampTransformer( const QgsColorRampTransformer &other );
%Docstring
Copy constructor
%End

//     QgsColorRampTransformer &operator=( const QgsColorRampTransformer &other );
    virtual Type transformerType() const;
    virtual QgsColorRampTransformer *clone() /Factory/;
    virtual QVariant toVariant() const;
    virtual bool loadVariant( const QVariant &definition );
    virtual QVariant transform( const QgsExpressionContext &context, const QVariant &value ) const;
    virtual QString toExpression( const QString &baseExpression ) const;

    QColor color( double value ) const;
%Docstring
 Calculates the color corresponding to a specific value.
 \param value value to calculate color for
 :return: calculated color using transformer's parameters and type
 :rtype: QColor
%End

    QgsColorRamp *colorRamp() const;
%Docstring
 Returns the color ramp used for calculating property colors.
 :return: color ramp
 \see setColorRamp()
 :rtype: QgsColorRamp
%End

    void setColorRamp( QgsColorRamp *ramp /Transfer/ );
%Docstring
 Sets the color ramp to use for calculating property colors.
 \param ramp color ramp, ownership of ramp is transferred to the transformer.
 \see colorRamp()
%End

    QColor nullColor() const;
%Docstring
 Returns the color corresponding to a null value.
 \see setNullColor()
 :rtype: QColor
%End

    void setNullColor( const QColor &color );
%Docstring
 Sets the color corresponding to a null value.
 \param color null color
 \see nullSize()
%End

    QString rampName() const;
%Docstring
 Returns the color ramp's name.
 \see setRampName()
 :rtype: str
%End

    void setRampName( const QString &name );
%Docstring
 Sets the color ramp's ``name``. The ramp name must be set to match
 a color ramp available in the style database for conversion to expression
 to work correctly.
 \see rampName()
%End

};

/************************************************************************
 * This file has been generated automatically from                      *
 *                                                                      *
 * src/core/qgspropertytransformer.h                                    *
 *                                                                      *
 * Do not edit manually ! Edit header and run scripts/sipify.pl again   *
 ************************************************************************/
