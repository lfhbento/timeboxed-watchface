import FastClick from 'react-fastclick-alt';
import Field from './field';
import React, { PropTypes } from 'react';
import classnames from 'classnames';

class RadioButtonGroup extends React.Component {
    constructor(props) {
        super(props);
        this.state = {
            selectedItem: typeof props.selectedItem !== 'undefined' ? props.selectedItem : props.options['0'].value,
        };
    }

    render() {
        return (
            <Field fieldName={this.props.fieldName} label={this.props.label} labelPosition={this.props.labelPosition}>
                <FastClick>
                    <div className="btn-group">
                        {this.props.options.map((item) => {
                            let classes = {
                                'btn btn-outline-primary': true,
                                active: this.state.selectedItem === item.value,
                            };
                            if (this.props.size) {
                                classes[this.props.size] = true;
                            }
                            return (
                                <button
                                    key={item.value}
                                    className={classnames(classes)}
                                    onClick={this.toggleButton.bind(this, item.value)}>
                                    {item.label}
                                </button>
                            );
                        })}
                    </div>
                </FastClick>
            </Field>
        );
    }

    toggleButton(index) {
        this.setState({ selectedItem: index });
        if (this.props.onChange) {
            this.props.onChange(index);
        }
    }
}

RadioButtonGroup.propTypes = {
    selectedItem: PropTypes.string,
    options: PropTypes.arrayOf(PropTypes.object),
    onChange: PropTypes.func,
    name: PropTypes.string,
    fieldName: PropTypes.string,
    label: PropTypes.string,
    labelPosition: PropTypes.string,
    size: PropTypes.string,
};

export default RadioButtonGroup;
