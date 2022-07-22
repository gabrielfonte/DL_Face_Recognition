import { Log } from './log';

describe('Log', () => {
  it('should create an instance', () => {
    expect(new Log("XX:XX:XX:XX:XX", 0, 0, 0)).toBeTruthy();
  });
});
