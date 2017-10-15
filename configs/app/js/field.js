import React, { PropTypes } from 'react';
import classnames from 'classnames';

class Field extends React.Component {
    render() {
        let classes = classnames({
            [`field field--${this.props.fieldName}`]: true,
            'field--long-label': this.props.longLabel,
            'field--long': this.props.labelPosition === 'top',
            'field--long-reverse': this.props.labelPosition === 'bottom',
        });
        return (
            <div className="field-container">
                <div className={classes}>
                    {this.props.label ? <label className="field-label">{this.props.label}</label> : null}
                    <div className="field-content">
                        <div className="field-content-inner">{this.props.children}</div>
                    </div>
                </div>
            </div>
        );
    }
}

Field.propTypes = {
    fieldName: PropTypes.string.isRequired,
    label: PropTypes.string,
    longLabel: PropTypes.bool,
    labelPosition: PropTypes.string,
    children: PropTypes.any,
};

export default Field;
