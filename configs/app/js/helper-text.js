/* eslint-disable react/no-danger */
import React, { PropTypes } from 'react';
import classnames from 'classnames';

const HelperText = (props) => {
    let classes = classnames({
        'helper-text': true,
        'helper-text--field': !props.standalone,
        'helper-text--standalone': props.standalone,
    });
    return <div className={classes} dangerouslySetInnerHTML={{ __html: props.children }} />;
};

HelperText.propTypes = {
    standalone: PropTypes.bool,
    children: PropTypes.any,
};

HelperText.defaultProps = {};

export default HelperText;
