import Field from './field';
import React, { PropTypes } from 'react';
import Select from 'react-select';

class DropdownField extends React.Component {
    constructor(props) {
        super(props);
        this.state = {
            selectedItem:
                typeof props.selectedItem !== 'undefined'
                    ? props.selectedItem
                    : props.clearable ? null : props.options[0].value,
        };
        this.onChange = this.onChange.bind(this);
    }

    onChange(value) {
        if (this.props.onChange) {
            this.props.onChange(value);
        }
        this.setState({ selectedItem: value ? value.value : null });
    }

    componentWillReceiveProps(nextProps) {
        this.setState({ selectedItem: nextProps.selectedItem });
    }

    render() {
        return (
            <Field fieldName={this.props.fieldName} label={this.props.label} labelPosition={this.props.labelPosition}>
                <Select
                    className="dropdown"
                    options={this.props.options}
                    name={this.props.name}
                    value={this.state.selectedItem}
                    onChange={this.onChange}
                    searchable={this.props.searchable}
                    clearable={this.props.clearable}
                    placeholder={this.props.searchable ? 'Type to search...' : 'Select...'}
                    multi={false}
                />
            </Field>
        );
    }
}

DropdownField.propTypes = {
    selectedItem: PropTypes.string,
    clearable: PropTypes.bool,
    options: PropTypes.arrayOf(PropTypes.object),
    onChange: PropTypes.func,
    name: PropTypes.string,
    fieldName: PropTypes.string,
    label: PropTypes.string,
    labelPosition: PropTypes.string,
    searchable: PropTypes.bool,
};

DropdownField.defaultProps = {
    searchable: false,
    clearable: false,
};

export default DropdownField;
