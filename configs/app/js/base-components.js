import { getFirstKeyFromObject } from './util/util';
import React, { PropTypes } from 'react';

class SelectableComponent extends React.Component {
    constructor(props) {
        super(props);
        this.state = {
            selectedItem:
                typeof props.selectedItem !== 'undefined' ? props.selectedItem : getFirstKeyFromObject(props.options),
        };
    }
}

SelectableComponent.propTypes = {
    selectedItem: PropTypes.number,
    options: PropTypes.arrayOf(PropTypes.object).isRequired,
    fieldName: PropTypes.string.isRequired,
    label: PropTypes.string,
};

export { SelectableComponent };
