import { TestBed } from '@angular/core/testing';

import { LogStoreService } from './log-store.service';

describe('LogStoreService', () => {
  let service: LogStoreService;

  beforeEach(() => {
    TestBed.configureTestingModule({});
    service = TestBed.inject(LogStoreService);
  });

  it('should be created', () => {
    expect(service).toBeTruthy();
  });
});
