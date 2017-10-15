import React, { PropTypes } from 'react';

const SideBySideFields = (props) => {
    return (
        <div className="side-by-side">
            <div className="side-by-side--left">{props.children[0]}</div>
            <div className="side-by-side--right">{props.children[1]}</div>
        </div>
    );
};

SideBySideFields.propTypes = {
    children: PropTypes.any,
};

SideBySideFields.defaultProps = {};

export default SideBySideFields;
