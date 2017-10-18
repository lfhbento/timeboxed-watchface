import { getConfigs, getReturnUrl } from './util/util';
import LZString from './util/lz-string';
import Layout from './layout';
import React from 'react';
import ReactDOM from 'react-dom';
import es6Promise from 'es6-promise';

es6Promise.polyfill();

try {
    const onSubmit = (data) => {
        document.location = getReturnUrl() + LZString.compressToBase64(JSON.stringify(formatDataToSend(data)));
    };

    const getStoredData = (source) => {
        return Object.keys(source).reduce((data, key) => {
            let value = source[key] === undefined ? '' : source[key];

            value = value === 'true' || value === 'false' ? JSON.parse(value) : value;
            value = typeof value === 'string' && value.indexOf('0x') !== -1 ? value.replace('0x', '#') : value;

            return Object.assign(data, { [key]: value });
        }, {});
    };

    const getStoredDataFromParams = () => {
        let config = getConfigs();
        config = config ? JSON.parse(config) : {};
        return getStoredData(config);
    };
    const formatDataToSend = (data) => {
        let newData = Object.keys(data).reduce((items, key) => {
            items[key] = data[key];

            if (key.indexOf('Color') !== -1) {
                items[key] = items[key].replace('#', '0x');
            }

            return items;
        }, {});

        return newData;
    };

    ReactDOM.render(
        <Layout onSubmit={onSubmit} state={getStoredDataFromParams()} />,
        document.getElementById('content')
    );
} catch (ex) {
    alert(ex.stack);
}
