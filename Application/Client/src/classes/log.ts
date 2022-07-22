export class Log {
    constructor(
        public mac: string, 
        public messages_qtd: number,
        public warnings_qtd: number,
        public errors_qtd: number,
        public messages?: string[],
        public warnings?: string[],
        public errors?: string[], 
        ) {
    }	
}
