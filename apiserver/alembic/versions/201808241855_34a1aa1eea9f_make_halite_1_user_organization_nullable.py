"""Make Halite 1 user organization nullable

Revision ID: 34a1aa1eea9f
Revises: f090029de9e3
Create Date: 2018-08-24 18:55:09.090607+00:00

"""
from alembic import op
import sqlalchemy as sa
from sqlalchemy.dialects import postgresql


# revision identifiers, used by Alembic.
revision = '34a1aa1eea9f'
down_revision = 'f090029de9e3'
branch_labels = None
depends_on = None


def upgrade():
    op.alter_column('halite_1_user', 'organization', nullable=True)


def downgrade():
    op.alter_column('halite_1_user', 'organization', nullable=False)
