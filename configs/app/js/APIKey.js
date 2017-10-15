import { getText } from './lang';
import HelperText from './helper-text';
import React, { PropTypes } from 'react';
import TextField from './text-field';

const APIKey = (props, context) => {
    return (
        <div>
            <TextField
                fieldName={props.keyName}
                label={getText(context.locale, 'API Key')}
                value={props.value}
                onChange={props.onChange}
            />
            <HelperText>{props.helperText}</HelperText>
        </div>
    );
};

APIKey.propTypes = {
    value: PropTypes.string,
    onChange: PropTypes.func,
    keyName: PropTypes.string,
    helperText: PropTypes.string,
};
APIKey.defaultProps = {};
APIKey.contextTypes = {
    locale: PropTypes.string,
};

export default APIKey;
