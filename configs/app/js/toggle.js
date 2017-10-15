import FastClick from 'react-fastclick-alt';
import Field from './field';
import React, { PropTypes } from 'react';
import Toggle from 'react-input-toggle/dist/react-input-toggle';
import 'react-input-toggle/lib/styles/switch.scss';

class ToggleField extends React.Component {
    constructor(props) {
        super(props);
        this.state = { checked: !!props.checked };
        this.toggleCheckbox = this.toggleCheckbox.bind(this);
    }

    render() {
        return (
            <Field label={this.props.label} fieldName={this.props.fieldName} longLabel={true}>
                <label>
                    <FastClick>
                        <Toggle effect="bbounce" onChange={this.toggleCheckbox} checked={this.state.checked} />
                    </FastClick>
                </label>
            </Field>
        );
    }

    componentWillReceiveProps(nextProps) {
        if (this.props.checked !== nextProps.checked) {
            this.toggleCheckbox({ target: { checked: !!nextProps.checked } });
        }
    }

    toggleCheckbox(e) {
        let value = !!e.target.checked;
        this.setState({ checked: value });
        if (this.props.onChange) {
            this.props.onChange(value);
        }
    }
}

ToggleField.propTypes = {
    checked: PropTypes.bool,
    fieldName: PropTypes.string.isRequired,
    label: PropTypes.string,
    onChange: PropTypes.func,
};

ToggleField.defaultProps = {
    checked: false,
};

export default ToggleField;
