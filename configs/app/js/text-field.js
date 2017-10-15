import Field from './field';
import React, { Component, PropTypes } from 'react';

class TextField extends Component {
    constructor(props) {
        super(props);

        this.onChange = this.onChange.bind(this);
        this.onButtonClick = this.onButtonClick.bind(this);
    }

    render() {
        return (
            <Field
                fieldName={this.props.fieldName}
                label={this.props.label}
                labelPosition={this.props.labelPosition}
                helperText={this.props.helperText}>
                <div className="field-text">
                    <input
                        type="text"
                        className="form-control"
                        name={this.props.fieldName}
                        value={this.props.value}
                        onChange={this.onChange}
                    />
                    {this.props.buttonLabel ? (
                        <button className="btn btn-primary field-text--btn" onClick={this.onButtonClick}>
                            {this.props.buttonLabel}
                        </button>
                    ) : null}
                </div>
            </Field>
        );
    }

    onChange(e) {
        let value = e.target.value;
        if (this.props.onChange) {
            this.props.onChange(value);
        }
    }

    onButtonClick() {
        if (this.props.onButtonClick) {
            this.props.onButtonClick(this.props.value);
        }
    }
}

TextField.propTypes = {
    fieldName: PropTypes.string,
    label: PropTypes.string,
    labelPosition: PropTypes.string,
    helperText: PropTypes.string,
    value: PropTypes.string,
    buttonLabel: PropTypes.string,
    onChange: PropTypes.func,
    onButtonClick: PropTypes.func,
};

TextField.defaultProps = {};

export default TextField;
