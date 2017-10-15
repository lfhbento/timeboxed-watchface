import React, { PropTypes } from 'react';

const OptionGroup = (props) => {
    return (
        <div className="card card--custom">
            {props.title ? <h3 className="card-header">{props.title}</h3> : null}
            <div className="card-block card-block--custom">{props.children}</div>
        </div>
    );
};

OptionGroup.propTypes = {
    title: PropTypes.string,
    children: PropTypes.any,
};

OptionGroup.defaultProps = {};

export default OptionGroup;
