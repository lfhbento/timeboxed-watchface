import React, { PropTypes } from 'react';

const shouldShow = (version, minVersion, maxVersion) => {
    minVersion = minVersion || version;
    maxVersion = maxVersion || version;
    return isNewer(version, minVersion) && isNewer(maxVersion, version);
};

const isNewer = (a, b) => {
    let versionA = a.split('.').map((item) => parseInt(item, 10));
    let versionB = b.split('.').map((item) => parseInt(item, 10));
    return versionA[0] > versionB[0] || (versionA[0] === versionB[0] && versionA[1] >= versionB[1]);
};

const Versioned = (props) => {
    return shouldShow(props.version, props.minVersion, props.maxVersion) ? <div>{props.children}</div> : null;
};

Versioned.propTypes = {
    children: PropTypes.any,
    version: PropTypes.string,
    minVersion: PropTypes.string,
    maxVersion: PropTypes.string,
};

Versioned.defaultProps = {};

export { shouldShow };

export default Versioned;
