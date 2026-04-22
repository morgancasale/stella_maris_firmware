class Opts extends HTMLElement{
    constructor() {
        // Always call super first in constructor
        super();
    }

    connectedCallback(){
        const deviceName = this.getAttribute("deviceName");
        const shadow = this.attachShadow({ mode: "open" });

        const options = document.createElement("md-menu");
        options.className = "options";
        options.id = "options_" + deviceName;

        const anchorEl = window.document.querySelector("#"+deviceName).shadowRoot.querySelector("#optsButton_"+deviceName);

        options.anchorElement = anchorEl;

        options.innerHTML = `
            <!-- <md-menu-item style="">
                <div style="float: left;">Name</div>
                <md-outlined-text-field
                    id="em_input"
                    class="em_input"
                    style="
                        float: left;
                        resize: none;
                        width: 200px;
                        margin-left: 5px;
                    "
                    label="Input" 
                    type="text"
                    rows= "1"
                >
                </md-outlined-text-field>
            </md-menu-item>
            <md-menu-item>
                <div style="float: left;">EM</div>
                <md-outlined-text-field
                    id="em_input"
                    class="em_input"
                    style="
                        float: left;
                        resize: none;
                        width: 102px;
                        margin-left: 5px;
                    "
                    label="Power" 
                    type="number"
                    rows= "1"
                >
                </md-outlined-text-field>
            </md-menu-item> -->
            <md-menu-item>
                <div slot="headline" onclick="disconnectDevice('` + deviceName + `')">Disconnect</div>
            </md-menu-item>
        `;

        const style = document.createElement("style");
        style.textContent = `
            .btn_container{
                margin: 10px;
                width: max-content;
                height: 2em;
            }
            .deviceButton{
                margin-right: 2.5px;
            }
            .em_input{
                resize: none;
                width: 102px;
            }
        `;

        shadow.appendChild(options);

        // Fire an event when the menu is closed
        const observer = new MutationObserver((mutationsList) => {
            for (const mutation of mutationsList) {
              if (mutation.attributeName === 'aria-hidden') {
                // Fire a custom event when 'aria-hidden' changes
                this.dispatchEvent(
                    new CustomEvent('ariahiddenchange', {
                        bubbles: true,
                        composed: true,
                        detail: { newValue: options.getAttribute('aria-hidden') }
                    })
                );
              }
            }
        });
        observer.observe(options, { attributes: true });

        // Prevent the input from closing the menu
        // const em_input = this.shadowRoot.querySelector("#em_input");

        // em_input.addEventListener('click', (event) => {
        //     event.stopPropagation();
        // });
        // em_input.addEventListener('mousedown', (event) => {
        //     event.stopPropagation();
        // });

        // Fire an event when this component is rendered
        const event = new CustomEvent('opts-rendered', { bubbles: true, composed: true });
        this.dispatchEvent(event);
    }

    

    static get observedAttributes() {
        return ["deviceName"];
    }
}

customElements.define('opts-menu', Opts);