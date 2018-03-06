#pragma once
#include "acdocman.h"

class GLockDocument
{
public:
	GLockDocument()
	{
		pDoc = NULL;
		if (acDocManager != NULL)
		{
			try
			{
				pDoc = acDocManager->curDocument();
				if ((pDoc != NULL) && (acDocManager->lockDocument(pDoc) != Acad::eOk))
				{
					pDoc = NULL;
				}
			}
			catch (...)
			{}
		}
	}

	~GLockDocument()
	{
		if ((acDocManager != NULL) && (pDoc != NULL))
		{
			try
			{
				acDocManager->unlockDocument(pDoc);	
			}
			catch (...)
			{}
		}
	}

private:
	AcApDocument* pDoc;
};